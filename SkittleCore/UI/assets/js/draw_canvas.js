
var init = function() {
    imageObj = new Image();
    imageObj.src = nd_url; // source data
    imageRMap = new Image();
    imageRMap.src = rm_url; // source data


    graphRMvisible = false;

    c.imageSmoothingEnabled = false; // so it won't be blury when it scales
    c.webkitImageSmoothingEnabled = false;
    c.mozImageSmoothingEnabled = false;
    c.scale(Math.round(3*zoom),Math.round(3*zoom))
    imageObj.onload = function(){
        drawGraphs() 
    }

    if (document.defaultView && document.defaultView.getComputedStyle) {
      stylePaddingLeft = parseInt(document.defaultView.getComputedStyle(cc, null)['paddingLeft'], 10)      || 0;
      stylePaddingTop  = parseInt(document.defaultView.getComputedStyle(cc, null)['paddingTop'], 10)       || 0;
      styleBorderLeft  = parseInt(document.defaultView.getComputedStyle(cc, null)['borderLeftWidth'], 10)  || 0;
      styleBorderTop   = parseInt(document.defaultView.getComputedStyle(cc, null)['borderTopWidth'], 10)   || 0;
    }
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

// the part that does the actual work
var drawGraphs = function() {
    drawNucDisplay()
    if(graphRMvisible) drawRMap();

    c.clearRect(0,0,2000,1000) // render on visible canvas (which has scale applied)
    c.drawImage(b.canvas, 0, 0);
}
var drawNucDisplay = function() {   
    b.clearRect(0,0,1000,1000)
    b.drawImage(imageObj,0,0) // render data on hidden canvas

    var imageData = b.getImageData(0, 0, imageObj.width, imageObj.height);
    var data = imageData.data;
    var newImageData = b.createImageData(width,1000/(3*zoom)) //create new image data with desired dimentions (width)
    var newData = newImageData.data;
    for (var x = 0; x < newData.length; x += 4*scale) { // read in data from original pixel by pixel
        var y = x + (start - 1)*4 - width*4*10; // adjust for start offset
        newData[x] = data[y] || 0;
        newData[x + 1] = data[y + 1] || 0;
        newData[x + 2] = data[y + 2] || 0;
        newData[x + 3] = data[y + 3] || 0;
    }
    b.clearRect(0,0,10000,10000)
    b.putImageData(newImageData, 0, 0);

}
var drawRMap = function() {
    b.drawImage(imageRMap,width+12,Math.round(-start/width + 10))
}

// UI Dials interaction
var UIwidthChange = function(newWidth) {
    // var newWidth = this.value
    if (isNaN(newWidth / 1) == false) { // check if this is really just a number
        setWidthTo(newWidth);
    }
    else {
        console.log('Width input is not a valid number')
        // updateWidth(width); //reset input to current value
    }
}
var updateWidth = function(newWidth) {
    $('#widthDisplay').text(newWidth + " bp")
    $('.widthBased').width(width*zoom*3 + 20);
    updateEnd();
}
var UIstartChange = function(newStart) {
    // var newStart = this.value
    if (isNaN(newStart / 1) == false) { // check if this is really just a number
        setStartTo(newStart)
    }
    else {
        console.log('Start index input is not a valid number')
        //updateStart(start); //reset input to current value
    }
}
var updateStart = function(newStart) {
    $('#startDisplay').text(newStart)
    updateEnd();
}
var UIendChange = function(newEnd) {
    if (isNaN(newEnd / 1) == false) { // check if this is really just a number
        setStartTo(newEnd - $('#canvasContainer').height()*width/3 + width*3*10)
    }
    else {
        console.log('Start index input is not a valid number')
        //updateStart(start); //reset input to current value
    }
}
var updateEnd = function() {
    $('#endDisplay').text(Math.round(start + $('#canvasContainer').height()*width/3 - width*3*10) )
}

// setters and setter utilities
var setStartTo = function(newStart) {
    if (newStart < 1) {
        start = 1;
    }
    else if (newStart >140001) {
        start = 140001;
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
    else if (newWidth >1000) {
        width = 1000;
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
var toggleRM = function(requestor) {
    if (requestor.checked) {
        graphRMvisible = true
        $('#graphLabelRM').show()
        drawGraphs();
    }
    else {
        graphRMvisible = false
        $('#graphLabelRM').hide()
        drawGraphs();
    }
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
    widthInPixels = width*zoom*3
    if(activeTool == "Move") {
        if (mx > widthInPixels && mx < (widthInPixels+50) ) {
            isDrag = dragWidth = true
            edgeOffset = mx - widthInPixels
            this.style.cursor = 'col-resize'
        }
        else {
            topOffset = my;
            startOffset = start;
            isDrag = true;
            this.style.cursor = 'move'

        }
    }

}
function mouseMove(e) {
    getMouseLocation(e)
    widthInPixels = width*zoom*3
    if(activeTool == "Move") {
        if (isDrag && dragWidth){
            if (mx<1) { //lose the drag if mouse goes over the edge
                mouseUp(e)
            }
            setWidthTo( Math.round( (mx-edgeOffset)/(zoom*3) ) )
        }
        else if (isDrag) {

            setStartTo( Math.round( (topOffset-my)/(zoom*3) ) * width + startOffset )
        }
        else if(mx > widthInPixels && mx < (widthInPixels+50) ) {
            this.style.cursor = 'col-resize'
        }
        else {
            this.style.cursor = 'default'
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