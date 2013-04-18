
var keyListener = function(e) {
    if($('input[type="text"]:focus').length == 0) { // if no text inputs are being used
        if(!e){
           //for IE
           e = window.event;
        }
        if(e.keyCode==37){ //left arrow
            state.width(by(-1))
        }
        if(e.keyCode==39){ //right arrow
            state.width(by(1))
        }
        if(e.keyCode==38){ //up arrow
            e.preventDefault()
            state.start(by(lines(-10)))
        }
        if(e.keyCode==40){ //down arrow
            e.preventDefault() //don't scroll the page
            state.start(by(lines(10)))
        }
        if(e.keyCode==191){ // /(slash)
            e.preventDefault() /
            state.width(half)
        }
        if(e.keyCode==88 || e.keyCode==190) { // x or .
            e.preventDefault() 
            state.width(double)
        }
        if(e.keyCode==219){ // [
            e.preventDefault() 
            state.start(1);
        }

        if(e.keyCode==221){ // ]
            e.preventDefault() 
            state.goToEnd()
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
    clearSelectedAnnotation()

    if(graphStatus["a"] && graphStatus["a"].visible && (activeTool == "Move" || activeTool == "Select") )  {
        if(mx < toPixels(graphStatus["a"].skixelOffset +graphStatus["a"].skixelWidth) && mx > toPixels(graphStatus["a"].skixelOffset) ) {
            var column = calcAnnotationColumn(mx)
            var row = toSkixels(my)
            $.each(visibleAnnotations,function(i,v){
                if(column == annotations[v].column) {
                        // console.log(column,row)
                    if((row+1) >= annotations[v].startRow && (row-1) <= (annotations[v].startRow + annotations[v].rowHeight)) {
                        annotationSelectedStart = annotations[v]["Start"]
                        annotationSelectedEnd = annotations[v]["End"]
                        if(annotations[activeAnnotation]) annotations[activeAnnotation].active = false
                        activeAnnotation = v
                        annotations[activeAnnotation].active = true;

                        annotations[v].snp_name = v

                        showAnnotationDetail(annotations[v]);

                        isInvalidDisplay = true;
                        return false; //aka break
                    }
                }
            })
        }
    }
    if(activeTool == "Move") {
        if (graphStatus["n"].visible) {
            var leftSideOfClickZone = toPixels(graphStatus["n"].skixelOffset + state.width())
                if (mx > leftSideOfClickZone && mx < (leftSideOfClickZone + toPixels(gutterWidth)) ) { //change width
                    dragWidth = true
                    edgeOffset = mx - leftSideOfClickZone
                    this.style.cursor = 'col-resize'
                }
            }

    //else { // scroll
        isDrag = true;
        topOffset = my;
        startOffset = state.start();
        leftOffset = mx;
        sideOffset = xOffset
        this.style.cursor = 'move'
    //}
    }
    else if (activeTool == "Select") {
        var x = Math.max(0,Math.min(state.width(),(toSkixels(mx)-graphStatus["n"].skixelOffset+1)))

        var selectionStart = state.start() + (toSkixels(my-25))*state.bpPerLine() + x*state.scale()
        var selectionEnd = selectionStart + state.bpPerLine() - 1;
        getRawSequence(selectionStart,selectionEnd,function(seq){
            var newSeq = $('#highlighterSequence').clone().removeAttr('id').addClass('highlighterSequence').insertBefore('.addSeq')
            newSeq.find('.sequenceInput').val(seq)
            newSeq.find('.sequenceColor').val(getGoodDeterministicColor(seq).slice(1))
            newSeq.find('.showSeq').prop('checked', true);
            graphStatus['h'].controls = true;
        })
        // console.log('selection start:' + selectionStart + " selection end:" + selectionEnd)
        showGraph('h');
        if (graphStatus['h'].visible) isInvalidDisplay = true
    }

}
function mouseMove(e) {
    getMouseLocation(e)
    if(activeAnnotation==0 && graphStatus["a"] && graphStatus["a"].visible && (activeTool == "Move" || activeTool == "Select") )  {
        if(mx < toPixels(graphStatus["a"].skixelOffset +graphStatus["a"].skixelWidth) && mx > toPixels(graphStatus["a"].skixelOffset) ) {
            var column = calcAnnotationColumn(mx)
            var row = toSkixels(my)
            $.each(visibleAnnotations,function(i,v){
                if(annotations[v]) annotations[v].active = false
                if(column == annotations[v].column) {
                    if((row+1) >= annotations[v].startRow && (row-1) <= (annotations[v].startRow + annotations[v].rowHeight)) {
                        annotationSelectedStart = annotations[v]["Start"]
                        annotationSelectedEnd = annotations[v]["End"]
                        annotations[v].active = true;
                        isInvalidDisplay = true;
                        return false;
                    }
                }

                activeAnnotation = annotationSelectedStart = annotationSelectedEnd = 0;
                isInvalidDisplay = true;
            })
        }
    }
    if(activeTool == "Move") {
        if (graphStatus["n"].visible) { //dragging width only applies to Nuc Display
            var leftSideOfClickZone = toPixels(graphStatus["n"].skixelOffset + state.width())

            // var widthInPixels = toPixels(width)
                if (dragWidth){
                    if (mx < 1) { //lose the drag if mouse goes over the edge
                        mouseUp(e)
                        return;
                    }
                    state.width( toSkixels(mx - edgeOffset) - graphStatus["n"].skixelOffset )
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

            state.start( toSkixels(topOffset-my) * state.bpPerLine() + startOffset )
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
        state.start(by(lines(-delta*10)));
    }
}
function mouseWheelDials(e) {
    e.preventDefault();
    var delta = calcDeltaFromScrollEvent(event)

    if (delta > 0.05 || delta < -0.05) {
        switch($(this).children('span').attr('id')) {
            case "widthDisplay": state.width(by(-delta*10)); return;
            case "scaleDisplay": state.scale(by(-delta*5)); return;
            default: state.start(by(-delta*10*state.scale())); return;
        }
    }
}
// html widgets
$(function() {

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
        linkPopover.html('Copy this link: <br><span>' + getCurrentPageURL('fullURL') + '</span>');
        selectText(linkPopover.children('span')[0])
        setTimeout(function() {
          linkPopover.removeClass('active');
        }, 1500);
    })
    $('#bug-report').click(function(){
        $('#feedback_current_view').val(getCurrentPageURL('fullURL'))
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
        closeSettings(graph)
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
    $('#graph-labels .settingsGraphButton').click(function() {
        var graph = this.parentNode.id.slice(-1);
        if ( $('#settingsLabel-'+graph).length > 0 ) closeSettings(graph)
        else {
            settingsLabel
                .clone()
                .attr('id', 'settingsLabel-'+graph)
                .insertAfter($(this).parent())
                .children('.closeSettingsButton').click(function() {
                    var graph = this.parentNode.id.slice(-1);
                    closeSettings(graph);
                })
            settingsGraph(graph);
        }
    })

    $('#graphLabel-h .graphSettings').empty().append($('.highlighterSettings').detach())
    $('.highlighterSettings').on('change blur click','input,#searchSeq',function(){
        graphStatus['h'].settings = hSettingsFromUI();
        isInvalidDisplay = true;
    })
    $('.highlighterSettings .addSeq').on('click',function(){
        var newSequenceEntry = $('#highlighterSequence').clone().removeAttr('id').addClass('highlighterSequence');
        newSequenceEntry.find('.sequenceColor').val(getGoodDeterministicColor(Math.random()))
        newSequenceEntry.insertBefore($(this))

    })
    $('.highlighterSettings').on('click','.removeSeq',function(){
        $(this).parent().remove();
        graphStatus['h'].settings = hSettingsFromUI();
        isInvalidDisplay = true;
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
        inputBox.blur(function() {
            if(typeof state[targetFunction] != 'undefined' && state[targetFunction] instanceof Function) {
                state[targetFunction](this.value)
            } 
            updateDials();
            $(this).remove();
        })
    })

});




var helpGraph = function(graph) {
    graphStatus[graph].help = true;
    $('#graphLabel-'+graph+" .graphHelp").addClass('active');
    isInvalidDisplay = true;
}
var closeHelp = function(graph) {
    graphStatus[graph].help = false;
    $('#graphLabel-'+graph+" .graphHelp").removeClass('active');
    $('#helpLabel-'+graph).remove()
    isInvalidDisplay = true;
}
var settingsGraph = function(graph) {
    graphStatus[graph].controls = true;
    $('#graphLabel-'+graph+" .graphSettings").addClass('active');
    isInvalidDisplay = true;
}
var closeSettings = function(graph) {
    graphStatus[graph].controls = false;
    $('#graphLabel-'+graph+" .graphSettings").removeClass('active');
    $('#settingsLabel-'+graph).remove()
    isInvalidDisplay = true;
}

var hSettingsFromUI = function(){  // TODO: Validate
    var hState = {};
    hState.revComplement = $('#revComplement').is(':checked')? true : false;
    hState.similarityPercent = Math.max(20,Math.min(100,$('#similarityPercent').val()));
    hState.sequences = [];
    $('.highlighterSequence').each(function(i){
        var validSequence = $(this).find('.sequenceInput').val().match(/([acgtn]+)/i)[0] || "aaaaaaaa";
        var validColor = $(this).find('.sequenceColor').val().match(/[0-9a-f]+/gi)[0] || "00ff00";
        hState.sequences[i] = {
            'show':$(this).find('.showSeq').is(':checked'),
            'sequence':validSequence,
            'color':validColor
        };
    })
    return hState;
}
var highlighterEncodeURL = function(hState) {
    if (typeof hState != 'object' || !hState.sequences) return "";
    var s = ""
    if (hState.revComplement) s += "&rev";
    s += "&sim=" + hState.similarityPercent
    $.each(hState.sequences,function(i,v){
        if (v.show) {
            if(v.sequence) s += "&s" + i + "=" + v.sequence
            if(v.color) s += "&s" + i + "c=" + v.color
        }
    })
    return s
}
var loadHighlighterSettings = function(hState) {
    if (typeof hState != 'object') return false;
    hState.revComplement = hState.revComplement || hState.searchReverseComplement;
    $('#revComplement').prop('checked', hState.revComplement);
    hState.similarityPercent = hState.similarityPercent || hState.minimumPercentage*100;
    $('#similarityPercent').val(hState.similarityPercent);
    if (hState.targetSequenceEntries) hState.sequences = hState.targetSequenceEntries
    $('.highlighterSequence').remove()
    $.each(hState.sequences,function(i,v){
        var seq = $('#highlighterSequence').clone().removeAttr('id').addClass('highlighterSequence')
        var sequence = v.sequence || v.seq;
        v.show = v.seq ? true : false;
        seq.find('.showSeq').prop('checked', v.show);
        seq.find('.sequenceInput').val(sequence);
        seq.find('.sequenceColor').val('#'+v.color)
        seq.insertBefore('.addSeq')
    })
}

var getCurrentPageURL = function(fullURL) {
    var graphString = ""
    for (var key in graphStatus) {
        if (graphStatus[key].visible == true) graphString += key;
    }
    var baseURL = (window.location.origin) ? window.location.origin : window.location.protocol + window.location.host;
    var currentURL = window.location.pathname + "?graphs=" + graphString + "&start=" + state.start() + "&scale=" + state.scale() + "&width=" + state.width() 
    if (graphStatus['h'].visible && graphStatus['h'].settings) currentURL += highlighterEncodeURL(graphStatus['h'].settings)
    // if (graphStatus['h'].visible) currentURL += "&searchStart=" + selectionStart + "&searchStop=" + selectionEnd;
    if (colorPalette !="Classic") graphPath += "&colorPalette="+colorPalette
    return fullURL ? baseURL + currentURL : currentURL;
}

var showAnnotationDetail = function (annotation) {
    var formatFunction = /^\d.*/.test(annotation.snp_name) ? formatGffDescription : formatSNPDescription;
    var popup = $('<div id="annotationDetail" />')
                    .addClass('popover active')
                    .html(formatFunction(annotation))

    var rowsBelowSelection = toSkixels($('#canvasContainer').height())-(Math.ceil(annotation.startRow)+annotation.rowHeight) //visible height - bottom of selection

    if( annotation.startRow < 100 && rowsBelowSelection < 100 ) {
        popup.addClass('arrow-left-top') 
        popup.css({ 'top':'30px',
                    'left': toPixels(graphStatus['n'].skixelOffset+state.width()+12)+'px'
                    })
    } 
    else if( annotation.startRow > rowsBelowSelection ) {
        popup.addClass('arrow-bottom-center') 
        popup.css({ 'bottom':($('#canvasContainer').height()-toPixels(Math.ceil(annotation.startRow))+16)+'px',
                    'left':(toPixels(graphStatus['n'].skixelOffset+state.width()/2)-150)+'px'
                    })
    } 
    else if (annotation.startRow < 0) {
    }
    else {
        popup.addClass('arrow-top-center')
        popup.css({ 'top':toPixels(Math.ceil(annotation.startRow)+annotation.rowHeight+6.3)+'px',
                    'left':(toPixels(graphStatus['n'].skixelOffset+state.width()/2)-150)+'px'
                    })
    }
    if(annotation["End"]-annotation["Start"] < state.bpPerLine() && annotation["Start"]-state.start() > 0) { //if selection is smaller than width
        popup.css({ 'left':(toPixels(graphStatus['n'].skixelOffset+((annotation["Start"]-state.start())/state.scale())%state.width())-150)+'px'})
    }

    $('#canvasContainer').append(popup)
}
var clearSelectedAnnotation = function() {
    $('#annotationDetail').remove()
    if(annotations[activeAnnotation]) annotations[activeAnnotation].active = false
    activeAnnotation = annotationSelectedStart = annotationSelectedEnd = 0;
    isInvalidDisplay = true;
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
    // updateEnd();
    updateScale();

    if(activeAnnotation!=0) {
        $('#annotationDetail').remove()
        showAnnotationDetail(annotations[activeAnnotation])
    }

    // window.history.replaceState(null,null,getCurrentPageURL())
}

var updateWidth = function() {
    $('#widthDisplay').text(state.width() + " skixels")
    updateEnd();
}
var updateScale = function() {
    $('#scaleDisplay').text(state.scale() + " bp/skixel")
    updateEnd();
}
var updateStart = function() {
    $('#startDisplay').text(state.start())
    updateEnd();
}
var updateEnd = function() {
    newEnd = Math.round( state.start() + (skixelsOnScreen - (25+37)*state.width() - 1)*state.scale() )
    if (newEnd > fileLength) $('#endDisplay').text(fileLength);
    else $('#endDisplay').text(newEnd)
}

var setXoffsetTo = function(newX) {
    newX = Math.round(newX)
    if (newX > 0) {
        newX = 0
    }
    xOffset = newX
    isInvalidDisplay = true;
    $('#graph-labels').css('margin-left', toPixels(newX) + "px")
}


var getterSetter = (function(){ //don't work in the global scope
    var process_validation = function(validation_params){
        if (validation_params) return function(val){ // do magjiks
            if (validation_params.filter && typeof val == 'string') val = val.match(validation_params.filter)[0]
            val = Math.round(val);
            if (validation_params.min) val = Math.max(val,validation_params.min);// || Math.max(val,validation_params.min())
            if (typeof validation_params.max == 'number') val = Math.min(val,validation_params.max)
            else if (typeof validation_params.max == 'function') val = Math.min(val,validation_params.max())
            return val;
        }
        return function(val){ return val; } //don't do any magic
    }
    var getterSetter = function(validation_params){
        var _val = (validation_params && validation_params.defaultVal) || undefined;
        var process = process_validation(validation_params);
        return function(val){
            if (val === undefined) return _val;

            if (typeof val == 'function'){ val = val(_val); }
            val = process(val);

            isInvalidDisplay = true;
            calcSkixelsOnScreen();

            return _val = val || _val;
        }
    }
    return getterSetter;
})();

var StateObject = function() {}
state = new StateObject();
StateObject.prototype.width = getterSetter({defaultVal:100, filter:/(\d+)/, min:1, max:1024})
StateObject.prototype.start = getterSetter({defaultVal:1, filter:/(\d+)/, min:1, max:function(){return fileLength - (skixelsOnScreen - (25+37)*state.width())*state.scale()/2 + 1}})
StateObject.prototype.scale = getterSetter({defaultVal:1, filter:/(\d+)/, min:1, max:5000})
StateObject.prototype.end = function(a){ return state.start(a.match(/(\d+)/)[0] - ( skixelsOnScreen - (25+37)*state.width() - 1 )*state.scale())}
StateObject.prototype.bpPerLine = function(){ return this.width() * this.scale()}
StateObject.prototype.goToEnd = function(){ return this.start(fileLength); }
StateObject.prototype.scaleToFile = function(){ 
    this.start(1);
    this.width(200);
    calcSkixelsOnScreen();
    var newScale = fileLength/(skixelsOnScreen-20*this.width());
    return newScale < 50 ? this.scale(newScale) : this.scale(round(newScale,50,"up"));
}
var double = function(a){ return a * 2; }
var half = function(a){ return a / 2; }
var lines = function(a){ return Math.round(a) * state.bpPerLine(); }
var by = function(d){ return function(a){ return a + d; }}
var random = function(a){ return Math.random()*fileLength;}

