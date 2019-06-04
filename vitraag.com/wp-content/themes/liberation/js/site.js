/* Javascript for Liberation */

// Based on the url highlight the navigation as current
function defineNav() {
    var url = window.location.href;

   switch(url) {
	case "http://ramblings.vitraag.com/" :
		$("#ramblings").addClass('current');
		break;
	case "http://www.vitraag.com/":
		$("#home").addClass('current');
		break;
	case "http://thekarkhana.vitraag.com/":
		$("#onstartups").addClass('current');
		break;
	case "http://www.vitraag.com/cv/":
		$("#cofounder").addClass('current');
		break;
	case "http://healthblog.vitraag.com/":
		$("#healthblog").addClass('current');
		break;
	default:
		break;
   } 
}

