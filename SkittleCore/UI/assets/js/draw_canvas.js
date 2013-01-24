var mainLoop = window.setInterval(function(){
    if(isInvalidDisplay) {
        isInvalidDisplay = false
        drawGraphs();
        updateDials();
    }
},50)

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
        isInvalidDisplay = true
    }

    if (document.defaultView && document.defaultView.getComputedStyle) {
      stylePaddingLeft = parseInt(document.defaultView.getComputedStyle(cc, null)['paddingLeft'], 10)      || 0;
      stylePaddingTop  = parseInt(document.defaultView.getComputedStyle(cc, null)['paddingTop'], 10)       || 0;
      styleBorderLeft  = parseInt(document.defaultView.getComputedStyle(cc, null)['borderLeftWidth'], 10)  || 0;
      styleBorderTop   = parseInt(document.defaultView.getComputedStyle(cc, null)['borderTopWidth'], 10)   || 0;
    }

    $(window).resize(function() {
        calcSkixelsOnScreen();
        updateEnd();
    });
    for (var i=0;i<graphOrder.length;i++) {
        $("#graphLabel-" + graphOrder[i]).appendTo("#graph-labels ul")
        $("#showGraph-" + graphOrder[i]).parent().appendTo("#graphList ul")
    }
}

var imageRequestor = function(graph,chunkOffset) {
    if (!imageObj[graph]) {
        imageObj[graph] = []
    }
    if (imageObj[graph]) {
        var graphPath = graphURL(graph,chunkOffset)
        if (!imageObj[graph][chunkOffset] 
            || ( imageObj[graph][chunkOffset].complete 
                && imageObj[graph][chunkOffset].source != graphPath ) ) {
            imageObj[graph][chunkOffset] = new Image();
            imageObj[graph][chunkOffset].source = graphPath;
            imageObj[graph][chunkOffset].src = graphPath;
            console.log("request " + graphPath)
            imageObj[graph][chunkOffset].onload = function() { // causes a little bit of jitter when scrolling
                console.log("onload "+imageObj[graph][chunkOffset].src)
                isInvalidDisplay = true
            }
            imageObj[graph][chunkOffset].onerror = function() {
                console.log('not a valid filename')
            }
        }
    }
    return imageObj[graph][chunkOffset]
}
var graphURL = function(graph,chunkOffset) {
    var startTopOfScreen = (start-8*width) >  0 ? (start-8*width) : 1
    var startChunk = ( ( Math.floor(startTopOfScreen/(65536*scale) ) + chunkOffset )*65536*scale + 1 );
    var graphPath = "data.png?graph=" + graph + "&start=" + startChunk + "&scale=" + scale;
    if (graphStatus[graph].rasterGraph != true) graphPath += "&width=" + Math.round(width/10)*10 
    graphPath += "&colorPalette="+colorPalette
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
    var offset = xOffset + gutterWidth
    var chunks = Math.min( Math.ceil(skixelsOnScreen/65536 + 1),(Math.ceil(fileLength/65536)-Math.floor((start-8*width)/65536)) )
    // for (key in graphStatus) {
    for (var i=0;i<graphOrder.length;i++) {
        var key = graphOrder[i];
        if (graphStatus[key].visible) {
            graphStatus[key].skixelOffset = offset;
            var skixelWidthofGraph = graphStatus[key].skixelWidth = drawGraph(key,offset,chunks);
            skixelWidthofGraph = Math.max(skixelWidthofGraph,toSkixels(minimumWidth))
            offset = offset + skixelWidthofGraph;
            $('#graphLabel-' + key).width(toPixels(skixelWidthofGraph));
        }
    }

    c.clearRect(0,0,2000,1000) // render on visible canvas (which has scale applied)
    c.drawImage(b.canvas, 0, 0);
}

var drawGraph = function(graph,offset,chunks) {
    switch (graph) {
        case "n": return drawNucDisplay(offset,chunks);
        case "b": return drawNucBias(offset,chunks);
        case "m": return drawRMap(offset,chunks);
        case "s": return drawSimHeat(offset,chunks);
        default: 
            console.log("Requested graph does not have a cooresponding javascript function, trying generic")
            return drawVerticalGraph(graph,offset,chunks);
    }
}
var drawVerticalGraph = function(graph,offset,chunks) {
    var graphWidth = 0
    for (var i=0;i<chunks;i++) {
        var imageObj = imageRequestor(graph,i)
        if(!imageObj.complete) imageObj = imageUnrendered;
        else var graphWidth = imageObj.width
        var vOffset = -Math.round(((start-8*width)%65536)/(width*scale) - i*(65536/width));
        b.drawImage(imageObj,offset,vOffset,graphWidth,Math.ceil(65536/width)) // render data on hidden canvas
    }
    return calculateOffsetWidth(graphWidth)
}
var drawAnnotations = function(offset,chunks) {
    var imageObj = imageRequestor('a',0)
    b.drawImage(imageObj,offset,Math.round(-start/width + 8))

    return calculateOffsetWidth(imageObj.width)
}
var drawNucDisplay = function(offset,chunks) {
    a.clearRect(0,0,1024,500)
    for (var i=0;i<chunks;i++) {
        var imageObj = imageRequestor("n",i)
        if(!imageObj.complete) imageObj = imageUnrendered;
        a.drawImage(imageObj,0,64*i) // render data on hidden canvas
    }

    var imageData = a.getImageData(0, 0, 1024, chunks*64);
    var data = imageData.data;
    var newImageData = b.createImageData(width,toSkixels(1000)) //create new image data with desired dimentions (width)
    var newData = newImageData.data;

    var startOffset = (start - 1 - width*8 - Math.max( Math.floor((start-width*8)/(65536*scale) ), 0 )*65536*scale )*4;
    for (var x = 0; x < newData.length; x += 4*scale) { // read in data from original pixel by pixel
        var y = x + startOffset
        newData[x] = data[y] || 0;
        newData[x + 1] = data[y + 1] || 0;
        newData[x + 2] = data[y + 2] || 0;
        newData[x + 3] = data[y + 3] || 0;
    }
    b.clearRect(0,0,10000,10000)
    b.putImageData(newImageData, offset, 0);

    return calculateOffsetWidth(width)
}
var drawNucBias = function(offset,chunks) {
    b.beginPath()
    b.rect(offset+20,0,20,500)
    b.fillStyle="#333";
    b.fill()

    drawVerticalGraph("b",offset,chunks)
    return calculateOffsetWidth(60)
}
var drawRMap = function(offset,chunks) {
    var fOffset = 0
    var fWidth = 61
    drawVerticalGraph("m",offset,chunks)
    // for (var i=0;i<chunks;i++) {
    //     var imageObj = imageRequestor("m",i)
    //     if(!imageObj.complete) imageObj = imageUnrendered;
    //     var vOffset = 8 - Math.round((start%65536)/(width*scale) - i*(65536/width));
    //     b.drawImage(imageObj,offset,vOffset,fWidth,(65536/width)) // render data on hidden canvas
    // }
    
    if (width<(fWidth-fOffset)) { //draw the red lines
        var widthPosition = offset+width-fOffset;
        b.beginPath();
        b.moveTo(widthPosition-1.5,0)
        b.lineTo(widthPosition-1.5,500)
        b.moveTo(widthPosition+0.5,0)
        b.lineTo(widthPosition+0.5,500)
        b.strokeStyle = "#f00"
        b.stroke();
    }
    return calculateOffsetWidth(fWidth)
}
var drawSimHeat = function(offset,chunks) {
    a.clearRect(0,0,300,1000)
    var displayWidth = 300
    var lineHeight = Math.round(65536/width);
    for (var i=0;i<chunks;i++) {
        var imageObj = imageRequestor("s",i)
        if(!imageObj.complete) imageObj = imageUnrendered;
        a.drawImage(imageObj,0,lineHeight*i,((Math.round(width/10)*10)/width*displayWidth),lineHeight) // render data on hidden canvas
    }
    var imageData = a.getImageData(0, 0, 300, chunks*lineHeight);
    var data = imageData.data;
    var newImageData = b.createImageData(displayWidth,displayWidth) //create new image data with desired dimentions (width)
    var newData = newImageData.data;

    // var startOffset = (start - 1 - width*8 - Math.max( Math.floor((start-width*8)/(65536*scale) ), 0 )*65536*scale )*4;

    var lineLength = displayWidth*4;
    var startOffset = (Math.round(start/width) - 8 - Math.max( Math.floor((start-width*8)/65536), 0 )*lineHeight)*lineLength
    var l = 0, i = startOffset
    for (var x = 0; x < newData.length; x += 4) { // read in data from original pixel by pixel
        var y = (x - l*lineLength)
        if (y >= lineLength) {
            l += 1
            x += 4*l
            i = startOffset + lineLength*l
        }
        var mirror = (y/4)*lineLength + l*4
        newData[x] = newData[mirror] = data[i] || 0;
        newData[x + 1] = newData[mirror + 1] = data[i + 1] || 0;
        newData[x + 2] = newData[mirror + 2] = data[i + 2] || 0;
        newData[x + 3] = newData[mirror + 3] = data[i + 3] || 0;
        i +=4
    }

    b.putImageData(newImageData, offset, 0);
    return calculateOffsetWidth(300)
    
}
var generatePlaceholderImage = function() {

}
