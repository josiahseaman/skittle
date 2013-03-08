Number.prototype.mod = function(n) {
	return ((this%n)+n)%n;
}
var toSkixels = function(pixels) {
    return Math.round(pixels/(3*zoom));
}
var toPixels = function(skixels) {
    return Math.round(skixels*3*zoom);
}
var calcSkixelsOnScreen = function() {
    skixelsOnScreen = toSkixels($('#canvasContainer').height())*width;
}
var calcDeltaFromScrollEvent = function(e) {
	var delta = 0
    if (e.wheelDelta) { /* IE/Opera. */
            delta = e.wheelDelta/120;
    } else if (e.detail) { /** Mozilla case. */
            delta = -e.detail/3;// In Mozilla, sign of delta is different than in IE. Also, delta is multiple of 3.
    }
    return delta;
}
function selectText(element) {
    var text = element;    

    if (document.body.createTextRange) { // ms
        var range = document.body.createTextRange();
        range.moveToElementText(text);
        range.select();
    } else if (window.getSelection) { // moz, opera, webkit
        var selection = window.getSelection();            
        var range = document.createRange();
        range.selectNodeContents(text);
        selection.removeAllRanges();
        selection.addRange(range);
    }
}
var expRound = function(value,stretchFactor) { // stretchFactor is between 0 to 1 (0 in not stretchy, 1 is very stretchy)
	if (stretchFactor<=0.01) return value;
	stretchFactor = 1/stretchFactor;
	var round = Math.pow(2,(Math.round(Math.log(Math.pow(value,stretchFactor))/Math.LN2)/stretchFactor))
	return Math.max(12,Math.round(round)); 
}

String.prototype.hashCode = function(){ // from http://werxltd.com/wp/2010/05/13/javascript-implementation-of-javas-string-hashcode-method/
	var hash = 0;
	if (this.length == 0) return hash;
	for (i = 0; i < this.length; i++) {
		char = this.charCodeAt(i);
		hash = ((hash<<5)-hash)+char;
		hash = hash & hash; // Convert to 32bit integer
	}
	return hash;
}

var getGoodDeterministicColor = function(input) {
	input = input + "padding"
	var hash = md5(input)
	var red = parseInt(hash.slice(1,3),16)
	var green = parseInt(hash.slice(3,5),16)
	var blue = parseInt(hash.slice(5,7),16)

	if (red < green && red < blue) red = 0
	else if (green < red && green < blue) green = 0
	else if (blue < red && blue < green) blue = 0
	else {
		if (hash%3 == 0) red = 0
		else if (hash%3==1) green = 0
		else blue = 0
	}
	for (var i=0;red+green+blue < 220;i++) {
		red = Math.min(red*2,255)
		green = Math.min(green*2,255)
		blue = Math.min(blue*2,255)
	}

	red=("0"+red.toString(16)).slice(-2)
	green=("0"+green.toString(16)).slice(-2)
	blue=("0"+blue.toString(16)).slice(-2)
	var color = "#" + red + green + blue
	return color
}

var formatGffDescription = function(annotationArray){
	var html =$('<div class="annotationDetail" />')
	var table = $('<table />')
	table.append($('<tr><th>Source:</th><td>'+annotationArray[0] + '</td></tr>'))
	table.append($('<tr><th>Feature:</th><td>'+annotationArray[1] + '</td></tr>'))
	table.append($('<tr><th>Start Index:</th><td>'+annotationArray[2] + '</td></tr>'))
	table.append($('<tr><th>End Index:</th><td>'+annotationArray[3] + '</td></tr>'))
	table.append($('<tr><th>Length:</th><td>'+(annotationArray[3]-annotationArray[2]+1) + 'bp</td></tr>'))
	if(annotationArray[4] !=null) table.append($('<tr><th>Score:</th><td>'+annotationArray[4] + '</td></tr>'))
	if(annotationArray[5] !=null) table.append($('<tr><th>Strand:</th><td>'+annotationArray[5] + '</td></tr>'))
	if(annotationArray[6] !=null) table.append($('<tr><th>Frame:</th><td>'+annotationArray[6] + '</td></tr>'))
	var descriptionArray = annotationArray[7].split(';')
	if (annotationArray.length>0) {
		$.each(descriptionArray,function(i,v){
			var keyValue = v.split('=',2)
			table.append($('<tr><th>'+keyValue[0]+':</th><td>'+keyValue[1] + '</td></tr>'))
		})
		html.append(table)
	} else {
		html.append(table)
		html.append($('<h4>Details:</h4><p>'+annotationArray[7]+'</p>'))
	}

	return html
}
var formatSNPDescription = function(annotationArray){
	var html =$('<div class="annotationDetail" />')
	var table = $('<table />')
	table.append($('<tr><th>SNP Code:</th><td>'+annotationArray.snp_name + '</td></tr>'))
	table.append($('<tr><th>Index:</th><td>'+annotationArray[2] + '</td></tr>'))
	table.append($('<tr><th>Details:</th><td><a href="https://www.23andme.com/you/explorer/snp/?snp_name='+annotationArray.snp_name + '" target="_blank">23andMe</a></td></tr>'))
	html.append(table)

	var table = $('<table class="SNPtable" />').append($('<tr><th>Reference</th><th>Mother</th><th>Father</th></tr>'))
	table.append($('<tr><td>?</td><td>'+colorfy(annotationArray[0])+'</td><td>'+colorfy(annotationArray[1]) + '</td></tr>'))

	html.append(table)

	return html
}
var calcAnnotationColumn = function(mx) {
	return Math.ceil((graphStatus["a"].skixelWidth+graphStatus["a"].skixelOffset-toSkixels(mx)-8)/3)
}
var benchmark = function(fn,count){
    var startTime = new Date().getTime();
    for (var i = 0; i < (count || 500); i++) {
        fn()
    };
    var endTime = new Date().getTime()
    return endTime - startTime
}
var colors = {"A":"#000000","C":"#ff0000","G":"#00ff00","T":"#0000ff","N":"#666666"}
var colorfy = function(nuc) {
	var nucArray = nuc.split('')
	var colorizedString = ""

	for (i in nucArray) {
		if(colors[nucArray[i]]) colorizedString += '<span class="nuc nuc-'+nucArray[i]+'">'+nucArray[i]+'</span>';
		else colorizedString += nucArray[i]
	}
	return colorizedString;
}
