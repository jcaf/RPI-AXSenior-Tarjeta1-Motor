"undefined"==typeof reviewsBadge&&(reviewsBadge=function(e,t){var n=document.getElementById(e),a="https://widget.reviews.co.uk/badge/widget",d="elementId="+e+"&version=1&";for(i in t)d.length>0&&(d+="&"),d+="object"!=typeof t[i]?i+"="+encodeURIComponent(t[i]):i+"="+encodeURIComponent(JSON.stringify(t[i]));a+="?"+d;var o=document.createElement("IFRAME");o.setAttribute("src",a),o.setAttribute("frameborder",0),o.width="100%",o.style.minWidth="140px",o.title="Reviews Badge Widget",n.appendChild(o);var r=function(t){try{var i=JSON.parse(t.data);if(void 0!=i.action)switch(i.action){case"widget-resize":i.elementId==e&&(o.height=i.height)}}catch(n){}};window.addEventListener?addEventListener("message",r,!1):attachEvent("onmessage",r)},"function"==typeof reviewsBadgeCallback&&reviewsBadgeCallback(),"function"==typeof badgeWidgetCallback&&badgeWidgetCallback());