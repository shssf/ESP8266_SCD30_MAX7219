#include "web_server.h"
#include "web_css.h"

static const char CSS_MAIN[] PROGMEM = R"CSS(
/* ------- Base & Typography ------- */
body{
  font-family:system-ui,Segoe UI,Roboto,Arial;
  margin:20px;
  background:#0f1014;
  color:#dcdfe4;
}
h1{
  font-size:20px;
  margin:0 0 12px;
}

/* ------- Cards ------- */
/* Base palette (no CSS variables to keep strings small on MCU) */
.card{
  position:relative;
  background:#181a1f;
  border:1px solid #262a33;
  border-radius:12px;
  padding:16px;
  margin-bottom:14px;
  /* Outer layered shadow for elevation */
  box-shadow:
    0 28px 50px rgba(0,0,0,0.55),
    0 2px 0 rgba(255,255,255,0.03) inset;
  /* Hover lift */
  transition:transform .12s ease, box-shadow .12s ease;
}
.card:hover{ transform:translateY(-1px); }
/* Subtle inner highlight around the edge */
.card:before{
  content:"";
  position:absolute;
  inset:0;
  border-radius:inherit;
  box-shadow:inset 0 1px 0 rgba(255,255,255,0.06);
  pointer-events:none;
}

/* Elevation levels */
.elev-1{
  box-shadow:
    0 16px 28px rgba(0,0,0,0.45),
    0 1px 0 rgba(255,255,255,0.03) inset;
}
.elev-2{
  box-shadow:
    0 24px 42px rgba(0,0,0,0.50),
    0 2px 0 rgba(255,255,255,0.03) inset;
}
.elev-3{
  box-shadow:
    0 32px 56px rgba(0,0,0,0.60),
    0 2px 0 rgba(255,255,255,0.03) inset;
}

/* ------- Buttons, Links, Status Pills ------- */
.btn{
  display:inline-block;
  padding:8px 12px;
  margin:4px;
  border:0;
  border-radius:8px;
  cursor:pointer;
  background:#121419;
  color:#e6e9ee;
}
.btn:hover{ background:#151720; }
.primary{ background:#3b82f6; color:#fff; }
.danger{  background:#ef4444; color:#fff; }
.status-pill{
  display:inline-block;
  padding:4px 8px;
  border-radius:999px;
  background:#252830;
  color:#eee;
  font-weight:600;
}
.link{ color:#9ec1ff; text-decoration:none; }
.link:hover{ text-decoration:underline; }

/* ------- Tables ------- */
table{
  width:100%;
  border-collapse:collapse;
  background:linear-gradient(180deg,#181a1f 0%,#15171c 100%);
  border:1px solid #262a33;
  border-radius:10px;
  overflow:hidden;
  box-shadow:0 20px 36px rgba(0,0,0,0.50);
}
th,td{
  padding:10px 12px;
  vertical-align:top;
  text-align:left;
}
th{
  background:#14161b;
  color:#e2e6ea;
  border-bottom:1px solid #2a2e38;
  font-weight:600;
}
td{
  color:#cfd3da;
  border-bottom:1px solid #23262f;
  box-shadow:inset 0 -1px 0 rgba(255,255,255,0.015);
}
tbody tr:hover{ background:#1a1d23; }
tbody tr:last-child td{ border-bottom:none; }

/* ------- Utilities & Grid ------- */
.mono{ font-family:ui-monospace,Menlo,Consolas,monospace; color:#b9d8ff; }
.hint{ color:#88909b; font-size:12px; }
.grid{ display:grid; grid-template-columns:1fr auto; gap:8px; }

/* ------- Charts/Canvas ------- */
.chart-card{
  background:#181a1fcc;
  border:1px solid #262a33;
  border-radius:12px;
}
.charts-row{ display:flex; gap:16px; flex-wrap:wrap; }
.chart-tile{
  position:relative;
  flex:1;
  min-width:320px;
  padding:8px;
  border-radius:12px;
  box-shadow:0 32px 56px rgba(0,0,0,0.60);
}
canvas{
  background:#0b0d12;
  border:1px solid #1f2330;
  border-radius:12px;
  width:100%;
  height:auto;
  display:block;
  filter:drop-shadow(0 8px 18px rgba(0,0,0,0.55));
}
.chart-label{
  position:absolute;
  top:6px;
  left:12px;
  color:#cfd3da;
  font-size:12px;
}
)CSS";

static void handle_css_main()
{
    http_send_P(200, "text/css", CSS_MAIN);
}

void ui_register_css_routes()
{
    http_route_get("/assets/main.css", handle_css_main);
}