
var init = function() {
    imageObj = {};
    imageObj["n"] = []
    imageObj["n"][0] = new Image();
    imageObj["n"][0].src = graphURL('n',0);
    // imageND = new Image()
    // imageND.src = nd_url; //graphURL("n");
    // imageRMap = new Image();
    // imageRMap.src = rm_url; // source data


    c.imageSmoothingEnabled = false; // so it won't be blury when it scales
    c.webkitImageSmoothingEnabled = false;
    c.mozImageSmoothingEnabled = false;
    c.scale(Math.round(3*zoom),Math.round(3*zoom))
    imageObj["n"][0].onload = function(){
        drawGraphs() 
    }

    if (document.defaultView && document.defaultView.getComputedStyle) {
      stylePaddingLeft = parseInt(document.defaultView.getComputedStyle(cc, null)['paddingLeft'], 10)      || 0;
      stylePaddingTop  = parseInt(document.defaultView.getComputedStyle(cc, null)['paddingTop'], 10)       || 0;
      styleBorderLeft  = parseInt(document.defaultView.getComputedStyle(cc, null)['borderLeftWidth'], 10)  || 0;
      styleBorderTop   = parseInt(document.defaultView.getComputedStyle(cc, null)['borderTopWidth'], 10)   || 0;
    }
}

var imageRequestor = function(graph,chunkOffset) {
    if (!imageObj[graph]) {
        imageObj[graph] = []
    }
    if (imageObj[graph]) {
        if (imageObj[graph][chunkOffset] 
            && imageObj[graph][chunkOffset].src == (window.location.origin + graphURL(graph,chunkOffset))) {
            return imageObj[graph][chunkOffset]
        }
        else {
            imageObj[graph][chunkOffset] = new Image();
            imageObj[graph][chunkOffset].src = graphURL(graph,chunkOffset);
            imageObj[graph][chunkOffset].onload = function() { // causes a little bit of jitter when scrolling
                drawGraphs() 
            }
            imageObj[graph][chunkOffset].onerror = function() {
                console.log('not a valid filename')
            }
            return imageObj[graph][chunkOffset]
        }
    }
    return imageND
}
var graphURL = function(graph,chunkOffset) {
    var startChunk = ( ( Math.floor(start/(65536*scale) ) + chunkOffset )*65536*scale + 1 );
    var graphPath = "/browse/data.png?graph=" + graph + "&start=" + startChunk + "&scale=" + scale 
    if (graph != "n") graphPath += "&width=" + width 
    return graphPath
}



// the part that does the actual work
var gutterWidth = 8 //skixels
var minimumWidth = 120 //pixels
var calculateOffsetWidth = function(skixelWidthofGraph) {
    return Math.max( (skixelWidthofGraph + gutterWidth), toSkixels(minimumWidth) )
}

var drawGraphs = function() {
    b.clearRect(0,0,1024,1000)
    var offset = gutterWidth

    for (key in graphStatus) {
        if (graphStatus[key].visible) {
            graphStatus[key].skixelOffset = offset;
            var skixelWidthofGraph = graphStatus[key].skixelWidth = eval( graphStatus[key].fn + "(" + offset + ")" );
            offset = offset + skixelWidthofGraph;
            $('#graphLabel-' + key).width( Math.max( (toPixels(skixelWidthofGraph)), minimumWidth ) );
        }
    }

    c.clearRect(0,0,2000,1000) // render on visible canvas (which has scale applied)
    c.drawImage(b.canvas, 0, 0);
}
var drawAnnotations = function(offset) {
    b.drawImage(imageAnnotations,offset,Math.round(-start/width + 8))

    return calculateOffsetWidth(imageRMap.width)
}
var drawNucDisplay = function(offset) {
    var chunks = toSkixels($('#canvasContainer').height()*width)/65536 + 1
    for (var i=0;i<chunks;i++) {
        var imageObj = imageRequestor("n",i)
        if(!imageObj.complete) imageObj = imageUnrendered;
        b.drawImage(imageObj,0,64*i) // render data on hidden canvas
    }
    

    var imageData = b.getImageData(0, 0, 1024, chunks*64);
    var data = imageData.data;
    var newImageData = b.createImageData(width,toSkixels(1000)) //create new image data with desired dimentions (width)
    var newData = newImageData.data;
    for (var x = 0; x < newData.length; x += 4*scale) { // read in data from original pixel by pixel
        var y = x + (start - 1 - ( Math.floor(start/(65536*scale) ) )*65536*scale )*4 - width*4*8; // adjust for start offset
        newData[x] = data[y] || 0;
        newData[x + 1] = data[y + 1] || 0;
        newData[x + 2] = data[y + 2] || 0;
        newData[x + 3] = data[y + 3] || 0;
    }
    b.clearRect(0,0,10000,10000)
    b.putImageData(newImageData, offset, 0);

    return calculateOffsetWidth(width)

}
var drawNucBias = function(offset) {
    var imageObj = imageRequestor('b',0)
    b.drawImage(imageObj,offset,Math.round(-start/width + 8))

    return calculateOffsetWidth(imageRMap.width)
}
var drawRMap = function(offset) {
    var chunks = toSkixels($('#canvasContainer').height()*width)/65536 + 1
    for (var i=0;i<chunks;i++) {
        var imageObj = imageRequestor("m",i)
        if(!imageObj.complete) imageObj = imageUnrendered;
        b.drawImage(imageObj,offset,Math.round(-start/width + 8)) // render data on hidden canvas
    }

    return calculateOffsetWidth(imageRMap.width)
}


