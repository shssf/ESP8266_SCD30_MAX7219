#include "web_server.h"
#include "web_ui_utils.h"


void handleOtaPage(void)
{
  String h;
  h.reserve(14000);

  h += ui_html_head("HTTP OTA");
  h += "<h1>Firmware upload (HTTP OTA)</h1>";

  /* STATUS BADGE */
  h += "<div class='card'><table>";
  h += "<tr><td>OTA state</td><td><span id='otaBadge' class='status-pill'>IDLE</span></td></tr>";
  h += "</table></div>";

  /* UPLOAD FORM */
  h += "<div class='card'>";
  h += "  <form id='otaForm' onsubmit='return submitOta(event)'>";
  h += "    <div class='grid'>";
  h += "      <input id='fileInput' type='file' name='update' accept='.bin,application/octet-stream' style='color:#ddd'>";
  h += "      <button id='uploadBtn' class='btn primary' type='submit'>Upload &amp; flash</button>";
  h += "    </div>";
  h += "    <p class='hint'>Auth: user <span class='mono'>admin</span>, password "
       "<span class='mono'>SETUP_AP_PASS</span>. If upload fails with 401, open <a class='mono' href='/update' target='_blank'>/update</a> "
       "to authorize once, then retry.</p>";
  h += "    <p class='hint'>Preflight: we will compare selected file size with <span class='mono'>/api/status.free_ota</span> before "
       "sending.</p>";
  h += "  </form>";
  h += "</div>";

  /* INDETERMINATE PROGRESS */
  h += "<div class='card' id='progressCard' style='display:none'>";
  h += "  <div style='margin-bottom:6px'>OTA progress</div>";
  h += "  <div style='height:14px;background:#222;border-radius:8px;overflow:hidden;position:relative'>";
  h += "    <div id='bar' style='height:100%;width:30%;background:#3b82f6;position:absolute;left:0'></div>";
  h += "  </div>";
  h += "  <div id='msg' class='hint' style='margin-top:6px'>Ready</div>";
  h += "</div>";

  /* BACK BUTTON */
  h += "<div><form method='GET' action='/'><button class='btn' type='submit'>Back</button></form></div>";

  /* JS: ACTIVE/IDLE badge, preflight size check, fetch upload, indeterminate bar */
  h += "<script>\n";
  h += "var anim=null;\n";
  h += "function setBadge(active){\n";
  h += "  var b=document.getElementById('otaBadge');\n";
  h += "  b.textContent = active ? 'ACTIVE' : 'IDLE';\n";
  h += "  b.style.background = active ? '#3b82f6' : '#333';\n";
  h += "}\n";
  h += "function startAnim(){\n";
  h += "  var bar=document.getElementById('bar'); var pos=0, dir=1;\n";
  h += "  anim=setInterval(function(){ pos+=dir*4; if(pos>70){dir=-1;} if(pos<0){dir=1;} bar.style.left=pos+'%'; }, 60);\n";
  h += "}\n";
  h += "function stopAnim(){ if(anim){ clearInterval(anim); anim=null; } }\n";
  h += "function preflightSizeOk(file){\n";
  h += "  return fetch('/api/status', {cache:'no-store'}).then(function(r){return r.json();}).then(function(j){\n";
  h += "    var free = (j && j.free_ota) ? j.free_ota : 0;\n";
  h += "    if(free>0 && file.size>free){\n";
  h += "      document.getElementById('msg').textContent='File too big ('+file.size+' B) > Free OTA space ('+free+' B)';\n";
  h += "      return false;\n";
  h += "    }\n";
  h += "    return true;\n";
  h += "  }).catch(function(e){ return true; });\n";
  h += "}\n";
  h += "function submitOta(ev){\n";
  h += "  ev.preventDefault();\n";
  h += "  var el=document.getElementById('fileInput'); var btn=document.getElementById('uploadBtn');\n";
  h += "  if(!el.files || el.files.length===0){ alert('Select a .bin first'); return false; }\n";
  h += "  var file = el.files[0];\n";
  h += "  preflightSizeOk(file).then(function(ok){\n";
  h += "    if(!ok){ return; }\n";
  h += "    document.getElementById('progressCard').style.display='block';\n";
  h += "    btn.disabled=true; setBadge(true); startAnim(); document.getElementById('msg').textContent='Uploading...';\n";
  h += "    var fd=new FormData(); fd.append('update', file);\n";
  h += "    fetch('/update', { method:'POST', body: fd }).then(function(resp){\n";
  h += "      if(resp.status===401){ document.getElementById('msg').textContent='Unauthorized (401). Open /update to authenticate, then "
       "retry.'; return ''; }\n";
  h += "      return resp.text();\n";
  h += "    }).then(function(txt){\n";
  h += "      stopAnim(); btn.disabled=false; setBadge(false);\n";
  h += "      document.getElementById('msg').textContent = txt ? txt : 'Upload finished.';\n";
  h += "    }).catch(function(e){\n";
  h += "      stopAnim(); btn.disabled=false; setBadge(false);\n";
  h += "      document.getElementById('msg').textContent='Network error';\n";
  h += "    });\n";
  h += "  });\n";
  h += "  return false;\n";
  h += "}\n";
  h += "</script>";

  h += ui_html_tail();
  http_send(200, "text/html; charset=utf-8", h.c_str());
}
