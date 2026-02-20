#include <Arduino.h>

#include "web_server.h"
#include "web_ui_pages.h"
#include "web_ui_utils.h"

#include "local_scd30.h"

static void handleApiScd30(void)
{
  char json[128];
  float s_last_co2 = 0.0f;
  float s_last_t = 0.0f;
  float s_last_rh = 0.0f;

  scd30_read_values(s_last_co2, s_last_t, s_last_rh);
  snprintf(json, sizeof(json), "{\"co2_ppm\":%.2f,\"temperature_c\":%.2f,\"humidity_pct\":%.2f}", s_last_co2, s_last_t, s_last_rh);
  http_send(200, "application/json; charset=utf-8", json);
}

static void handleScd30Page(void)
{
  String h;
  h.reserve(20000);
  h += ui_html_head("SCD30 - CO2 / Temperature / Humidity");
  h += "<h1>SCD30 - live</h1>";
  h += "<div><form action='/' method='get'><button class='btn' type='submit'>Back</button></form></div>";
  h += "<div class='card elev-2'><p>";
  h += "<a class='mono' href='/api/scd30'>/api/scd30</a> ";
  h += "</p></div>";
  h += "<div class='card elev-2'><div class='row'>";
  h += "<div class='tile'><div class='label'>CO\u2082 (ppm)</div><div id='co2' class='val mono'>--</div></div>";
  h += "<div class='tile'><div class='label'>Temperature (°C)</div><div id='t' class='val mono'>--</div></div>";
  h += "<div class='tile'><div class='label'>Humidity (%)</div><div id='rh' class='val mono'>--</div></div>";
  h += "</div><p class='hint'>Numeric values update every second.</p></div>";
  h += "<div class='card chart-card elev-3'><h2>Charts</h2><div class='row charts-row'>";
  h += "<div class='tile chart-tile'><div class='chart-label'>CO\u2082 (ppm)</div><canvas id='chart_co2' width='600' "
       "height='200'></canvas></div>";
  h += "<div class='tile chart-tile'><div class='chart-label'>Temperature (°C)</div><canvas id='chart_t' width='600' "
       "height='200'></canvas></div>";
  h += "<div class='tile chart-tile'><div class='chart-label'>Humidity (%)</div><canvas id='chart_rh' width='600' "
       "height='200'></canvas></div>";
  h += "</div><p class='hint'>Last 300 samples shown; responsive width.</p></div>";
  h += F(R"JS(
<script>
var elCo2=document.getElementById('co2');
var elT=document.getElementById('t');
var elRH=document.getElementById('rh');
var cCO2=document.getElementById('chart_co2');
var cT=document.getElementById('chart_t');
var cRH=document.getElementById('chart_rh');
var ctxCO2=cCO2.getContext('2d');
var ctxT=cT.getContext('2d');
var ctxRH=cRH.getContext('2d');
var MAX_POINTS=300; var dataCO2=[],dataT=[],dataRH=[];
function pushValue(a,v){a.push(v); if(a.length>MAX_POINTS)a.shift();}
function draw(ctx,data,color,label){var W=ctx.canvas.width,H=ctx.canvas.height;var L=48,R=14,T=18,B=28;ctx.clearRect(0,0,W,H);var mn=1e9,mx=-1e9;for(var i=0;i<data.length;++i){var v=data[i]; if(v<mn)mn=v; if(v>mx)mx=v;} if(mn===1e9){mn=0;mx=1;} var span=mx-mn; if(span<0.1)span=0.1; mn-=span*0.1; mx+=span*0.1; ctx.strokeStyle='#232634';ctx.lineWidth=1;ctx.beginPath();for(var i=0;i<=6;++i){var x=L+i*(W-L-R)/6;ctx.moveTo(x,T);ctx.lineTo(x,H-B);}for(var i=0;i<=4;++i){var y=T+i*(H-T-B)/4;ctx.moveTo(L,y);ctx.lineTo(W-R,y);}ctx.stroke();ctx.strokeStyle='#b9beca';ctx.beginPath();ctx.moveTo(L,T);ctx.lineTo(L,H-B);ctx.lineTo(W-R,H-B);ctx.stroke();ctx.fillStyle='#9aa0ac';ctx.font='12px system-ui';ctx.textAlign='right';for(var i=0;i<=4;++i){var y=T+i*(H-T-B)/4;var val=mx-(i*(mx-mn)/4);var digs=(Math.abs(val)<10)?2:0;ctx.fillText(val.toFixed(digs),L-6,y+4);}ctx.textAlign='left';ctx.fillStyle='#cfd3da';ctx.fillText(label,L+8,T+12); if(data.length===0)return; var grad=ctx.createLinearGradient(0,T,0,H-B);grad.addColorStop(0,color);grad.addColorStop(1,'rgba(158,193,255,0.00)');ctx.beginPath();for(var i=0;i<data.length;++i){var x=L+i*(W-L-R)/Math.max(1,data.length-1);var y=T+((mx-data[i])*(H-T-B)/(mx-mn)); if(i===0)ctx.moveTo(x,y);else ctx.lineTo(x,y);}ctx.lineTo(L+(W-L-R),H-B);ctx.lineTo(L,H-B);ctx.closePath();ctx.globalAlpha=0.16;ctx.fillStyle=grad;ctx.fill();ctx.globalAlpha=1.0;ctx.shadowBlur=10;ctx.shadowColor='rgba(62,130,246,0.55)';ctx.strokeStyle=color;ctx.lineWidth=2.5;ctx.beginPath();for(var i=0;i<data.length;++i){var x=L+i*(W-L-R)/Math.max(1,data.length-1);var y=T+((mx-data[i])*(H-T-B)/(mx-mn)); if(i===0)ctx.moveTo(x,y);else ctx.lineTo(x,y);}ctx.stroke();ctx.shadowBlur=0;}
function resize(){var tiles=document.querySelectorAll('.chart-tile');function W(tile){var w=tile.clientWidth-16;if(w<320)w=320;if(w>1200)w=1200;return w;} if(tiles[0]){cCO2.width=W(tiles[0]);cCO2.height=200;} if(tiles[1]){cT.width=W(tiles[1]);cT.height=200;} if(tiles[2]){cRH.width=W(tiles[2]);cRH.height=200;} draw(ctxCO2,dataCO2,'#22d3ee','CO\u2082 (ppm)'); draw(ctxT,dataT,'#a78bfa','Temperature (°C)'); draw(ctxRH,dataRH,'#34d399','Humidity (%)');}
window.addEventListener('resize',function(){resize();});
async function refresh(){try{var r=await fetch('/api/scd30',{cache:'no-store'});if(!r.ok)throw new Error('HTTP '+r.status);var j=await r.json();var c=(j.co2_ppm!==null?+j.co2_ppm:null);var tc=(j.temperature_c!==null?+j.temperature_c:null);var rh=(j.humidity_pct!==null?+j.humidity_pct:null); elCo2.textContent=(c!==null&&isFinite(c))?c.toFixed(0):'--'; elT.textContent=(tc!==null&&isFinite(tc))?tc.toFixed(2):'--'; elRH.textContent=(rh!==null&&isFinite(rh))?rh.toFixed(1):'--'; if(c!==null&&isFinite(c))pushValue(dataCO2,c); if(tc!==null&&isFinite(tc))pushValue(dataT,tc); if(rh!==null&&isFinite(rh))pushValue(dataRH,rh); resize(); }catch(e){elCo2.textContent='ERR'; elT.textContent='ERR'; elRH.textContent='ERR';}}
(function(){refresh();resize();setInterval(refresh,1000);})();
</script>
)JS");
  h += ui_html_tail();
  http_send(200, "text/html; charset=utf-8", h.c_str());
}

void ui_register_scd30_routes(void)
{
  http_route_get("/scd30", handleScd30Page);
  http_route_get("/api/scd30", handleApiScd30);
}
