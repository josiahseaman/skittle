
var init = function() {
    imageObj = {};
    imageObj["n"] = []
    imageObj["n"][0] = new Image();
    imageObj["n"][0].src = graphURL('n',0);
    imageND = new Image()
    imageND.src = nd_url; //graphURL("n");
    imageRMap = new Image();
    imageRMap.src = rm_url; // source data


    c.imageSmoothingEnabled = false; // so it won't be blury when it scales
    c.webkitImageSmoothingEnabled = false;
    c.mozImageSmoothingEnabled = false;
    c.scale(Math.round(3*zoom),Math.round(3*zoom))
    imageND.onload = function(){
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
            // imageObj[graph][chunkOffset].onload = function() {
                // drawGraphs() 
            // }
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
    var graphPath = "/browse/hg19_chrY-sample_" + graph + "_" + startChunk + "_" + scale 
    if (graph != "n") graphPath += "_" + width 
    graphPath += ".png";
    return graphPath
}

var keyListener = function(e) {
    if($('input[type="text"]:focus').length == 0) { // if no text inputs are being used
        if(!e){
           //for IE
           e = window.event;
        }
        if(e.keyCode==37){ //left arrow
           changeWidthBy(-1)
        }
        if(e.keyCode==39){ //right arrow
           changeWidthBy(1)
        }
        if(e.keyCode==38){ //up arrow
            e.preventDefault()
           changeStartByLines(-10)
        }
        if(e.keyCode==40){ //down arrow
            e.preventDefault() //don't scroll the page
           changeStartByLines(10)
        }
    }

}

var toSkixels = function(pixels) {
    return Math.round(pixels/(3*zoom));
}
var toPixels = function(skixels) {
    return Math.round(skixels*3*zoom);
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
    b.drawImage(imageRMap,offset,Math.round(-start/width + 8))

    return calculateOffsetWidth(imageRMap.width)
}

// UI Dials interaction
var hideGraph = function(graph) {
    if (graphStatus[graph]) graphStatus[graph].visible = false;
    $('#showGraph-' + graph).prop('checked',false)
    $('#graphLabel-' + graph).hide();
    drawGraphs();
}
var showGraph = function(graph) {
    if (graphStatus[graph]) graphStatus[graph].visible = true;
    $('#graphLabel-' + graph).show();
    $('#showGraph-' + graph).prop('checked',true)
    drawGraphs();
}

var UIwidthChange = function(newWidth) {
    // var newWidth = this.value
    if (isNaN(newWidth / 1) == false) { // check if this is really just a number
        setWidthTo(newWidth);
    }
    else {
        console.log('Width input is not a valid number')
    }
}
var updateWidth = function(newWidth) {
    $('#widthDisplay').text(newWidth + " bp")
    updateEnd();
}
var UIstartChange = function(newStart) {
    // var newStart = this.value
    if (isNaN(newStart / 1) == false) { // check if this is really just a number
        setStartTo(newStart)
    }
    else {
        console.log('Start index input is not a valid number')
    }
}
var updateStart = function(newStart) {
    $('#startDisplay').text(newStart)
    updateEnd();
}
var UIendChange = function(newEnd) {
    if (isNaN(newEnd / 1) == false) { // check if this is really just a number
        setStartTo(newEnd - toSkixels($('#canvasContainer').height()*width) + toPixels(width*10))
    }
    else {
        console.log('Start index input is not a valid number')
    }
}
var updateEnd = function() {
    $('#endDisplay').text(Math.round(start + toSkixels($('#canvasContainer').height()*width) - toPixels(width*10) ) )
}

// setters and setter utilities

var setStartTo = function(newStart) {
    if (newStart < 1) {
        start = 1;
    }
    else if (newStart >1400001) {
        start = 1400001;
    }
    else {
        start = Math.round(newStart); // don't allow non-integer starts
    }
    drawGraphs();
    updateStart(start)
}
var setWidthTo = function(newWidth) {
    if (newWidth < 1) {
        width = 1;
    }
    else if (newWidth >1024) {
        width = 1024;
    }
    else {
        width = Math.round(newWidth); // don't allow non-integer widths
    }
    drawGraphs();
    updateWidth(width);
}
var changeWidthBy = function(delta) {
    setWidthTo(width + delta)
}
var doubleWidth = function() {
    setWidthTo(width*2)
}
var halfWidth = function() {
    setWidthTo(Math.round(width/2))
}
var changeStartBy = function(delta) {
    setStartTo(start + delta)
}
var changeStartByLines = function(deltaLines) {
    setStartTo(start + deltaLines*width)
}
var goToEnd = function() {
    setStartTo(100000000)
}

// mouse stuffs
var mx, my, edgeOffset, topOffset, startOffset; // mouse coordinates

function getMouseLocation(e) {
      var element = cc, offsetX = 0, offsetY = 0;

      if (element.offsetParent) {
        do {
          offsetX += element.offsetLeft;
          offsetY += element.offsetTop;
        } while ((element = element.offsetParent));
      }

      // Add padding and border style widths to offset
      offsetX += stylePaddingLeft;
      offsetY += stylePaddingTop;

      offsetX += styleBorderLeft;
      offsetY += styleBorderTop;

      mx = e.pageX - offsetX;
      my = e.pageY - offsetY
}

function mouseDown(e) {
    getMouseLocation(e);
    if(activeTool == "Move") {
        if (graphStatus["n"].visible) {
            var leftSideOfClickZone = toPixels(graphStatus["n"].skixelOffset + width)
                if (mx > leftSideOfClickZone && mx < (leftSideOfClickZone + toPixels(gutterWidth)) ) { //change width
                    dragWidth = true
                    edgeOffset = mx - leftSideOfClickZone
                    this.style.cursor = 'col-resize'
                }
            }

    //else { // scroll
        isDrag = true;
        topOffset = my;
        startOffset = start;
        this.style.cursor = 'move'
    //}
    }

}
function mouseMove(e) {
    getMouseLocation(e)
    if(activeTool == "Move") {
        if (graphStatus["n"].visible) { //dragging width only applies to Nuc Display
            var leftSideOfClickZone = toPixels(graphStatus["n"].skixelOffset + width)

            // var widthInPixels = toPixels(width)
                if (dragWidth){
                    if (mx < 1) { //lose the drag if mouse goes over the edge
                        mouseUp(e)
                        return;
                    }
                    setWidthTo( toSkixels(mx - edgeOffset) - graphStatus["n"].skixelOffset )
                }
                else if(mx > leftSideOfClickZone && mx < (leftSideOfClickZone + toPixels(gutterWidth)) ) {
                    this.style.cursor = 'col-resize'
                }
                else {
                    this.style.cursor = 'default'
                }
            }

        if (isDrag) {

            setStartTo( toSkixels(topOffset-my) * width + startOffset )
        }
    }
}
function mouseUp(e) {
    isDrag = dragWidth = false;
}
function mouseWheel(e) {
    e.preventDefault();
    var delta = 0;
    if (event.wheelDelta) { /* IE/Opera. */
            delta = event.wheelDelta/120;
    } else if (event.detail) { /** Mozilla case. */
            delta = -event.detail/3;// In Mozilla, sign of delta is different than in IE. Also, delta is multiple of 3.
    }
    if (delta) {
        changeStartByLines(Math.round(-delta*10));
    }

}