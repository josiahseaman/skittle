
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
    else {
        if(e.keyCode==13) { //enter
            $('input[type="text"]:focus').blur()
        }
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
    else if (activeTool == "Select") {

        var selectionStart = start + (toSkixels(my-25)-1)*width
        var selectionEnd = selectionStart + width - 1;
        console.log('selection start:' + selectionStart + " selection end:" + selectionEnd)
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
    else if (activeTool == "Select") {
        this.style.cursor = 'crosshair'
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
// html widgets
  $(document).ready(function() {

    $('#tools .radio-tools').click(function() {
        $('#tools .radio-tools').removeClass('active');
        $(this).addClass('active');
        if ($(this).attr('data-tool')) activeTool = $(this).attr('data-tool')
    })

    $('#link-here').click(function() {


        var offset = $(this).offset();
        // $(this).toggleClass('active');
        var graphString = ""
        for (var key in graphStatus) {
            if (graphStatus[key].visible == true) graphString += key;
        }
        var currentURL = window.location.origin + "/browse/genome/chromosome/?graphs=" + graphString + "&start=" + start + "&scale=" + scale + "&width=" + width 
        if (typeof linkPopover === "undefined") {
            linkPopover = $('<div class="popover active"></div>');
            $('body').append(linkPopover);
        }
        else {
            linkPopover.toggle();
        }
        linkPopover.offset({ top: (offset.top - 10), left: (offset.left + $(this).outerWidth() + 16) })
        linkPopover.html('Copy this link: <br>' + currentURL);
        setTimeout(function() {
          linkPopover.removeClass('active');
        }, 1500);
    })
    $('#dials span').click(function() {
        var targetFunction = $(this).attr('data-fn')
        var offset = $(this).position();
        var inputBox = $('<input type="text">');
        $(this).parent().append(inputBox.offset(offset).val($(this).html()));
        inputBox.select();
        // inputBox.val().split(' ')[0].select();
        inputBox.blur(function() {
            if(targetFunction) eval(targetFunction + '("' + this.value + '")')
            $(this).remove();
        })
    })
    $("#buttonGraphs").click(function(){
        $("#graphList").toggleClass('active')
        setTimeout(function() {
          $("#graphList").removeClass('active');
        }, 1500);
    })
    $('#graphList input').click(function() {
        graph = this.id.slice(-1);
        if ( $(this).is(':checked') ) showGraph(graph);
        else hideGraph(graph)
    })
    $('#graph-labels .closeGraphButton').click(function() {
        graph = this.parentNode.id.slice(-1);
        hideGraph(graph)
    })

  });

// UI Dials interaction
var hideGraph = function(graph) {
    if (graphStatus[graph]) graphStatus[graph].visible = false;
    $('#showGraph-' + graph).prop('checked',false)
    $('#graphLabel-' + graph).hide();
    isInvalidDisplay = true;
}
var showGraph = function(graph) {
    if (graphStatus[graph]) graphStatus[graph].visible = true;
    $('#graphLabel-' + graph).show();
    $('#showGraph-' + graph).prop('checked',true)
    isInvalidDisplay = true;
}
var updateDials = function() {
    updateWidth();
    updateStart();
    updateEnd();
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
var updateWidth = function() {
    $('#widthDisplay').text(width + " bp")
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
var updateStart = function() {
    $('#startDisplay').text(start)
    updateEnd();
}
var UIendChange = function(newEnd) {
    if (isNaN(newEnd / 1) == false) { // check if this is really just a number
        setStartTo(newEnd - ( toSkixels($('#canvasContainer').height()-25-37)*width - 1 )*scale)
    }
    else {
        console.log('End index input is not a valid number')
    }
}
var updateEnd = function() {
    $('#endDisplay').text(Math.round( start + (toSkixels($('#canvasContainer').height()-25-37)*width - 1)*scale ))
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
    isInvalidDisplay = true;
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
    isInvalidDisplay = true;
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
