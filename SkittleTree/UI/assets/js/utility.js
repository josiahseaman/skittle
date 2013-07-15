Number.prototype.mod = function(n) {
	return ((this%n)+n)%n;
}
if (!window.location.getParameter ) { //for non Chrome browsers. See http://chuvash.eu/2012/01/11/get-url-parameters-with-javascript/
  window.location.getParameter = function(key) {
	function parseParams() {
		var params = {},
			e,
			a = /\+/g,  // Regex for replacing addition symbol with a space
			r = /([^&=]+)=?([^&]*)/g,
			d = function (s) { return decodeURIComponent(s.replace(a, " ")); },
			q = window.location.search.substring(1);

		while (e = r.exec(q))
			if (typeof params[d(e[1])] == 'array') params[d(e[1])].push(d(e[2]))
			else if (params[d(e[1])]) params[d(e[1])] = [params[d(e[1])],d(e[2])] 
			else params[d(e[1])] = d(e[2]);

		return params;
	}

	if (!this.queryStringParams)
		this.queryStringParams = parseParams(); 

	if (key) return this.queryStringParams[key];
	else return this.queryStringParams;
  };
}
var round = function(val,precision,direction) {
	precision = precision || 1;
	if (direction && direction == "down") return Math.floor(val/precision)*precision;
	else if (direction && direction == "up") return Math.ceil(val/precision)*precision;
	return Math.round(val/precision)*precision;
}
var toSkixels = function(pixels) {
    return Math.round(pixels/(3*zoom));
}
var toPixels = function(skixels) {
    return Math.round(skixels*3*zoom);
}
var calcSkixelsOnScreen = function() {
    return skixelsOnScreen = toSkixels($('#canvasContainer').height())*state.width();
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

var getGoodDeterministicColor = function(input) {
	if (typeof input == 'undefined' || typeof input == 'string' && input.length < 1) input = Math.random()
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

var formatGffDescription = function(annotation){
	var html =$('<div class="annotationDetail" />')
	var table = $('<table />')
	table.append($('<tr><th>Source:</th><td>'+annotation["Source"] + '</td></tr>'))
	table.append($('<tr><th>Feature:</th><td>'+annotation["Feature"] + '</td></tr>'))
	table.append($('<tr><th>Start Index:</th><td>'+annotation["Start"] + '</td></tr>'))
	table.append($('<tr><th>End Index:</th><td>'+annotation["End"] + '</td></tr>'))
	table.append($('<tr><th>Length:</th><td>'+(annotation["End"]-annotation["Start"]+1) + 'bp</td></tr>'))
	if(annotation["Score"] !=null) table.append($('<tr><th>Score:</th><td>'+annotation["Score"] + '</td></tr>'))
	if(annotation["Strand"] !=null) table.append($('<tr><th>Strand:</th><td>'+annotation["Strand"] + '</td></tr>'))
	if(annotation["Frame"] !=null) table.append($('<tr><th>Frame:</th><td>'+annotation["Frame"] + '</td></tr>'))
	var descriptionArray = annotation["Attribute"].join(';').split(';')
	if (descriptionArray.length>0) {
		$.each(descriptionArray,function(i,v){
			var keyValue = v.match(/([A-Za-z][A-Za-z0-9_]*)(=| )"?([^\s"]*)"?/)
			if (keyValue) {
				keyValue[1] = keyValue[1][0].toUpperCase() + keyValue[1].slice(1).replace(/_/g," ")
				if (keyValue[1] == "Gene name") keyValue[3] = '<a href="https://www.google.com/search?q='+keyValue[3]+'" target="_blank">'+keyValue[3]+"</a>"
				table.append($('<tr><th>'+keyValue[1]+':</th><td>'+keyValue[3] + '</td></tr>'))
			}
		})
		html.append(table)
	} else {
		html.append(table)
		html.append($('<h4>Details:</h4><p>'+annotation["Attribute"].toString()+'</p>'))
	}

	return html
}
var formatSNPDescription = function(annotationArray){
	var html =$('<div class="annotationDetail" />')
	var table = $('<table />')
	if (annotationArray.snp_name.match(/rs\d+/)) table.append($('<tr><th>SNP Code:</th><td><a href="http://www.ncbi.nlm.nih.gov/projects/SNP/snp_ref.cgi?rs='+annotationArray.snp_name.match(/rs(\d+)/)[1]+'" target="_blank" title="NCBI Report">'+annotationArray.snp_name + '</a></td></tr>'))
	else if (annotationArray.snp_name.match(/i\d+/)) table.append($('<tr><th>SNP Code:</th><td><a href="http://opensnp.org/snps/'+annotationArray.snp_name+'" target="_blank" title="openSNP information">'+annotationArray.snp_name + '</a></td></tr>'))
	else table.append($('<tr><th>SNP Code:</th><td>'+annotationArray.snp_name + '</td></tr>'))
	table.append($('<tr><th>Index:</th><td>'+annotationArray["Start"] + '</td></tr>'))
	table.append($('<tr><th>Details:</th><td><a href="https://www.23andme.com/you/explorer/snp/?snp_name='+annotationArray.snp_name + '" target="_blank">23andMe</a></td></tr>'))
	html.append(table)

	var table = $('<table class="SNPtable" />').append($('<tr><th>Reference</th><th>Allele 1</th><th>Allele 2</th></tr>'))
	table.append($('<tr><td>'+colorfy(getNucleotideValues(annotationArray["Start"]))+'</td><td>'+colorfy(annotationArray["Allele 1"])+'</td><td>'+colorfy(annotationArray["Allele 2"]) + '</td></tr>'))

	html.append(table)

	return html
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
	if (!nuc) return "";
	var nucArray = nuc.split('')
	var colorizedString = ""

	for (i in nucArray) {
		if(colors[nucArray[i]]) colorizedString += '<span class="nuc nuc-'+nucArray[i]+'">'+nucArray[i]+'</span>';
		else colorizedString += nucArray[i]
	}
	return colorizedString;
}
var getNucleotideValues = function(findStart,findEnd) {
	if(findEnd===undefined) findEnd = findStart
	var chunk = Math.floor(findStart/65536)*65536+1;
    var tempImage = new Image()
    tempImage.src = "data.png?graph=n&start=" + chunk + "&scale=1&colorPalette=Classic";
    var sequence = "";
    seq = tempImage.onload = function(){
	    a.clearRect(0,0,1024,64)
        a.drawImage(tempImage,0,0) 
        for (var i=0;i<(findEnd+1 - findStart);i++) {
			var y = Math.floor((findStart+i-chunk)/1024);
			var x = (findStart+i-chunk) - y*1024;
        	var pixel = a.getImageData(x,y,1,1).data
        	if (pixel[0] == 255) sequence += "C"
        	else if (pixel[1] == 255) sequence += "G"
        	else if (pixel[2] == 255) sequence += "T"
        	else if (pixel[1] == pixel[2] && pixel[2] >0 ) sequence += "N"
        	else sequence += "A"
        }
	    // console.log(sequence)
	    return sequence
    }

	return seq()
}

var getRawSequence = function(start,end,callback) {
	$.get('sequence.fa',{'queryStart':start,'queryStop':end},function(data){callback(data)})
}

var parseChromosomeFilename = function(fileName) {
    var known = ["chromosome", "ch", "chr", "chro", "chrom"];
    var parts = fileName.split(/[\-_\. ]/)
    var result;
    if (parts.length > 1) {
    	$.each(parts, function(i,part){
    		$.each(known, function(i,sample){
    			if (part.toLowerCase().indexOf(sample)>=0) {
    				result = part;
    				return false;
    			}
    			if (result) return false;
    		})
    	})
    	return result? result : parts[parts.length-1];
    }
    else {
    	parts = parts[0]
    	$.each(known, function(i,sample){
    		if (parts.toLowerCase().indexOf(sample)>=0)
    			var location = parts.toLowerCase().indexOf(sample);
    			result = parts.slice(location);
    			return false;
    	})
    	return result? result : parts;
    }
}
