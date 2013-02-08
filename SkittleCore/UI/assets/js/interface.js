
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
        if(e.keyCode==191){ // /(slash)
            e.preventDefault() /
            halfWidth()
        }
        if(e.keyCode==88){ // x
            e.preventDefault() 
            doubleWidth()
        }
        if(e.keyCode==219){ // [
            e.preventDefault() 
            setStartTo(1);
        }

        if(e.keyCode==221){ // ]
            e.preventDefault() 
            goToEnd()
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
        leftOffset = mx;
        sideOffset = xOffset
        this.style.cursor = 'move'
    //}
    }
    else if (activeTool == "Select") {
        selectionStart = start + (toSkixels(my-25))*width*scale
        selectionEnd = selectionStart + width*scale - 1;
        console.log('selection start:' + selectionStart + " selection end:" + selectionEnd)
        showGraph('h');
        if (graphStatus['h'].visible) isInvalidDisplay = true
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
                    this.style.cursor = 'col-resize'
                }
                else if(mx > leftSideOfClickZone && mx < (leftSideOfClickZone + toPixels(gutterWidth)) ) {
                    this.style.cursor = 'col-resize'
                }
                else {
                    this.style.cursor = 'default'
                }
            }

        if (isDrag) {

            setStartTo( toSkixels(topOffset-my) * width*scale + startOffset )
            if(!dragWidth) {
                setXoffsetTo(sideOffset - toSkixels(leftOffset-mx))
                this.style.cursor = 'move'
            }
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
    var delta = calcDeltaFromScrollEvent(event)

    if (delta > 0.05 || delta < -0.05) {
        changeStartByLines(Math.round(-delta*10));
    }
}
function mouseWheelDials(e) {
    e.preventDefault();
    var delta = calcDeltaFromScrollEvent(event)

    if (delta > 0.05 || delta < -0.05) {
        switch($(this).children('span').attr('id')) {
            case "widthDisplay": changeWidthBy(Math.round(-delta*10)); return;
            case "scaleDisplay": changeScaleBy(Math.round(-delta*5)); return;
            default: changeStartBy(Math.round(-delta*10)*scale); return;
        }
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
        if (typeof linkPopover === "undefined") {
            linkPopover = $('<div class="popover active"></div>');
            $('body').append(linkPopover);
        }
        else {
            linkPopover.toggle();
        }
        linkPopover.offset({ top: (offset.top - 10), left: (offset.left + $(this).outerWidth() + 16) })
        linkPopover.html('Copy this link: <br><span>' + getCurrentPageURL() + '</span>');
        selectText(linkPopover.children('span')[0])
        setTimeout(function() {
          linkPopover.removeClass('active');
        }, 1500);
    })
    $('#bug-report').click(function(){
        $('#feedback_current_view').val(getCurrentPageURL())
        $('#feedbackForm').toggle()
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
        var graph = this.parentNode.id.slice(-1);
        hideGraph(graph)
        closeHelp(graph)
    })
    $('#graph-labels .helpGraphButton').click(function() {
        var graph = this.parentNode.id.slice(-1);
        if ( $('#helpLabel-'+graph).length > 0 ) closeHelp(graph)
        else {
            helpLabel
                .clone()
                .attr('id', 'helpLabel-'+graph)
                .insertAfter($(this).parent())
                .children('.closeHelpButton').click(function() {
                    var graph = this.parentNode.id.slice(-1);
                    closeHelp(graph);
                })
            helpGraph(graph);
        }
    })
    $("#dials li").on('mouseleave touchstart',function(){
        var target = $(this).children('div').addClass('active')
        setTimeout(function() {
          target.removeClass('active');
        }, 200);
    })
    $('#dials span').click(function() {
        var targetFunction = $(this).attr('data-fn')
        var offset = $(this).position();
        var inputBox = $('<input type="text">');
        $(this).parent().append(inputBox.css('position','absolute').offset(offset).val($(this).html()));
        inputBox.select();
        // inputBox.val().split(' ')[0].select();
        inputBox.blur(function() {
            if(typeof window[targetFunction] != 'undefined' && window[targetFunction] instanceof Function) {
                window[targetFunction](this.value)
            } 
            $(this).remove();
        })
    })

  });

var helpGraph = function(graph) {
    graphStatus[graph].help = true;
    if (graphHelpContents[graph]) $('#graphLabel-'+graph+" .graphHelp").html(graphHelpContents[graph])
    $('#graphLabel-'+graph+" .graphHelp").addClass('active');
    isInvalidDisplay = true;
}
var closeHelp = function(graph) {
    graphStatus[graph].help = false;
    $('#graphLabel-'+graph+" .graphHelp").removeClass('active');
    $('#helpLabel-'+graph).remove()
    isInvalidDisplay = true;
}
var getCurrentPageURL = function() {
    var graphString = ""
    for (var key in graphStatus) {
        if (graphStatus[key].visible == true) graphString += key;
    }
    var currentURL = window.location.origin + "/browse/" + specimen + "/" + chromosome + "/?graphs=" + graphString + "&start=" + start + "&scale=" + scale + "&width=" + width 
    return currentURL
}

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
    updateScale();
}
var UIwidthChange = function(newWidth) {
    if (newWidth.match(/(\d+)/)) { // check if this is really just a number
        setWidthTo(newWidth.match(/(\d+)/)[0]);
    }
    else {
        console.log('Width input is not a valid number')
    }
}
var updateWidth = function() {
    $('#widthDisplay').text(width + " skixels")
    updateEnd();
}
var UIscaleChange = function(newScale) {
    if (newScale.match(/(\d+)/)) { // check if this is really just a number
        setScaleTo(newScale.match(/(\d+)/)[0])
    }
    else {
        console.log('Scale input is not a valid number')
    }
}
var updateScale = function() {
    $('#scaleDisplay').text(scale + " bp/pixel")
    updateEnd();
}
var UIstartChange = function(newStart) {
    if (newStart.match(/(\d+)/)) { // check if this is really just a number
        setStartTo(newStart.match(/(\d+)/)[0])
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
    if (newEnd.match(/(\d+)/)) { // check if this is really just a number
        setStartTo(newEnd.match(/(\d+)/)[0] - ( skixelsOnScreen - (25+37)*width - 1 )*scale)
    }
    else {
        console.log('End index input is not a valid number')
    }
}
var updateEnd = function() {
    newEnd = Math.round( start + (skixelsOnScreen - (25+37)*width - 1)*scale )
    if (newEnd > fileLength) $('#endDisplay').text(fileLength);
    else $('#endDisplay').text(newEnd)
}

// setters and setter utilities
var setXoffsetTo = function(newX) {
    newX = Math.round(newX)
    if (newX > 0) {
        newX = 0
    }
    xOffset = newX
    isInvalidDisplay = true;
    $('#graph-labels').css('margin-left', toPixels(newX) + "px")
}

var setStartTo = function(newStart) {
    if (newStart < 1) {
        if (start ==1) return;
        start = 1;
    }
    else if (newStart > fileLength/2 && newStart > (fileLength - (skixelsOnScreen - (25+37)*width)*scale/2)) {
        start = fileLength - (skixelsOnScreen - (25+37)*width)*scale/2 + 1;
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
    calcSkixelsOnScreen();
    isInvalidDisplay = true;
}
var setScaleTo = function(newScale) {
    if(newScale <1) {
        if (scale==1) return;
        scale = 1;
    }
    else if (newScale > 5000) scale = 5000;
    else scale = Math.round(newScale)
        
    calcSkixelsOnScreen();
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
var changeScaleBy = function(delta) {
    setScaleTo(scale + delta)
}
var changeStartBy = function(delta) {
    setStartTo(start + delta)
}
var changeStartByLines = function(deltaLines) {
    setStartTo(start + deltaLines*width*scale)
}
var goToEnd = function() {
    setStartTo(fileLength)
}
var scaleToFile = function() {
    setStartTo(1)
    setScaleTo(fileLength/(skixelsOnScreen-20*width))
}