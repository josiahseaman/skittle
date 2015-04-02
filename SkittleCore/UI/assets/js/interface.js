
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
function processTouchEvent(e) {
    if (e.touches.length <= 1) {
    // console.log(e)
        e.preventDefault();
        switch (e.type) {
            case "touchstart": mouseDown(e);break;
            case "touchmove": mouseMove(e);break;
            case "touchend": mouseUp(e);break;
        }
    }
}

function mouseDown(e) {
    getMouseLocation(e);

    clearSelectedAnnotation();
    annotationMouseHandling("Mouse Down");

    if(activeTool == "Move") {
        cc.style.cursor = 'move';
        
        //is mx in a gutter?
        $.each(graphStatus,function(i,v){
            var rightEdgeOfGraph = toPixels(v.skixelOffset+v.skixelWidth);
            if ( v.visible && mx > rightEdgeOfGraph && mx < rightEdgeOfGraph + toPixels(gutterWidth) ) {
                dragWidth = true;
                widthOffset = state.width();
                cc.style.cursor = 'col-resize';
                return false;
            }
        });
        // if so then save the current mx and width
        // if (graphStatus["n"].visible) {
        //     var leftSideOfClickZone = toPixels(graphStatus["n"].skixelOffset + state.width())
        //     if (mx > leftSideOfClickZone && mx < (leftSideOfClickZone + toPixels(gutterWidth)) ) { //change width
        //         dragWidth = true
        //         edgeOffset = mx - leftSideOfClickZone
        //         cc.style.cursor = 'col-resize'
        //     }
        // }

    //else { // scroll
        isDrag = true;
        topOffset = my;
        startOffset = state.start();
        leftOffset = mx;
        sideOffset = xOffset;
    //}
    }
    else if (activeTool == "Select") {
        var x = Math.max(0,Math.min(state.width(),(toSkixels(mx)-graphStatus["n"].skixelOffset+1)))

        var selectionStart = state.start() + (toSkixels(my-25))*state.bpPerLine() + x*state.scale()
        var selectionEnd = selectionStart + state.bpPerLine() - 1;
        getRawSequence(selectionStart,selectionEnd,addHighlighterSearch)
        // console.log('selection start:' + selectionStart + " selection end:" + selectionEnd)
        //showGraph('h');
        //if (graphStatus['h'].visible) isInvalidDisplay = true
    }

}
function mouseMove(e) {
    getMouseLocation(e)

    annotationMouseHandling()

    if(activeTool == "Move") {
        if (dragWidth){
            if (mx < 1) { //lose the drag if mouse goes over the edge
                mouseUp(e)
                return false;
            }
            state.width( toSkixels(mx - leftOffset) + widthOffset )
            cc.style.cursor = 'col-resize';
        }
        else {
            cc.style.cursor = 'default';
        }
        $.each(graphStatus,function(i,v){
            var rightEdgeOfGraph = toPixels(v.skixelOffset+v.skixelWidth);
            if ( v.visible && mx > rightEdgeOfGraph && mx < rightEdgeOfGraph + toPixels(gutterWidth) ) {
                cc.style.cursor = 'col-resize';
                return false;
            }
        });

        if (isDrag) {

            state.start( toSkixels(topOffset-my) * state.bpPerLine() + startOffset )
            if(!dragWidth) {
                setXoffsetTo(sideOffset - toSkixels(leftOffset-mx))
                cc.style.cursor = 'move'
            }
        }
    }
    else if (activeTool == "Select") {
        cc.style.cursor = 'crosshair'
    }
}
function mouseUp(e) {
    isDrag = dragWidth = false;
}
function mouseWheel(e) {
    e.preventDefault();

    if (isDrag) return false;

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
var annotationMouseHandling = function(isMouseDown) {
    if(activeAnnotation==0 && annotationStatus && (activeTool == "Move" || activeTool == "Select")) {
        var annotationFile,column,row
        $.each(annotationStatus,function(i,v) {
            if (v.visible==true && mx>toPixels(v.skixelOffset) && mx<toPixels(v.skixelOffset+v.skixelWidth)) {
                annotationFile = v.FileName;
                column = Math.ceil((v.skixelWidth+v.skixelOffset-toSkixels(mx)-8)/3)
                row = toSkixels(my)
                return false
            }
        })
        if (annotations[annotationFile]) {
            activeAnnotation = annotationSelectedStart = annotationSelectedEnd = 0;
            $.each(visibleAnnotations[annotationFile],function(i,v){
                var a = annotations[annotationFile][v]
                if(a) a.active = false;
                else return true;
                if(column == a.column) {
                    if((row+1) >= a.startRow && (row-1) <= (a.startRow + a.rowHeight)) {
                        annotationSelectedStart = a.Start
                        annotationSelectedEnd = a.End
                        a.active = true;
                        isInvalidDisplay = true;
                        if (isMouseDown) {
                            if(activeAnnotation.FileName && annotations[activeAnnotation.FileName][activeAnnotation.annotation]) {
                                annotations[activeAnnotation].active = false;
                            }
                            activeAnnotation = {"FileName":annotationFile,"annotation":v}
                            a.snp_name = v // only applies to SNPs

                            showAnnotationDetail(a);

                            return false; //aka break
                        }
                    }
                }
            });
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
        document.onkeydown = (typeof document.onkeydown == "function")? null : keyListener;
    })
    $("#buttonGraphs").click(function(){
        $("#graphList").toggleClass('active')
        setTimeout(function() {
          $("#graphList").removeClass('active');
        }, 1500);
    })
    $("#buttonAnnotations").click(function(){
        $("#annotationList").toggleClass('active')
        setTimeout(function() {
          $("#annotationList").removeClass('active');
        }, 1500);
    })
    $('#graphList input').click(function() {
        graph = this.id.slice(-1);
        if ( $(this).is(':checked') ) showGraph(graph);
        else hideGraph(graph)
    })
    $('#annotationList input').click(function() {
        graph = this.id.match(/showAnnotation-(.*)/)[1];
        if ( $(this).is(':checked') ) showGraph(graph);
        else hideGraph(graph)
    })
    $('#graph-labels .closeGraphButton').click(function() {
        var graph = this.parentNode.id.match(/graphLabel-(.*)/)[1];
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
            closeSettings(graph);
        }
    })
    $('#graph-labels .settingsGraphButton').click(function() {
        var graph = this.parentNode.id.slice(-1);
        if ( $('#settingsLabel-'+graph).length > 0 ) closeSettings(graph)
        else {

            settingsGraph(graph);
            closeHelp(graph);
        }
    })

    $('#graphLabel-h .graphSettings').empty().append($('.highlighterSettings').detach())
    $('.highlighterSettings').on('change blur click','input,#searchSeq',function(){
        graphStatus['h'].settings = hSettingsFromUI();
        isInvalidDisplay = true;
    })
    $('.highlighterSettings').on('change','#similarityPercent',function(){ 
        if ($(this).simpleSlider) { $('#similarityPercent').simpleSlider("setValue", $(this).val()); }
    })
    $('.highlighterSettings .addSeq').on('click',addHighlighterSearch)
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
    if (!graphStatus[graph]) return false;
    graphStatus[graph].help = false;
    $('#graphLabel-'+graph+" .graphHelp").removeClass('active');
    $('#helpLabel-'+graph).remove()
    isInvalidDisplay = true;
}
var settingsGraph = function(graph) {
    graphStatus[graph].controls = true;
    if ($('#settingsLabel-'+graph).size()==0) {
        settingsLabel
            .clone()
            .attr('id', 'settingsLabel-'+graph)
            .insertAfter($('#graphLabel-'+graph))
            .find('.closeSettingsButton').click(function() {
                var graph = this.parentNode.id.slice(-1);
                closeSettings(graph);
            })
    }
    $('#graphLabel-'+graph+" .graphSettings").addClass('active');
    if ($('.highlighterSequence').size() == 0) addHighlighterSearch('AAAAAAAAAA');
}
var closeSettings = function(graph) {
    if (!graphStatus[graph]) return false;
    graphStatus[graph].controls = false;
    $('#graphLabel-'+graph+" .graphSettings").removeClass('active');
    $('#settingsLabel-'+graph).remove()
    isInvalidDisplay = true;
}
var addHighlighterSearch = function(seq){
    var newSeq = $('#highlighterSequence').clone().removeAttr('id').addClass('highlighterSequence')
    if (typeof seq == 'string') {
        seq = seq.match(/([acgtn]+)/i)? seq.match(/([acgtn]+)/i)[0] : "";
        newSeq.find('.sequenceInput').val(seq.toUpperCase());
        newSeq.find('.showSeq').prop('checked', true);
    }
    else {
        seq = Math.random();
    }
    newSeq.find('.sequenceColor').val(getGoodDeterministicColor(seq))
    newSeq.insertBefore('.addSeq')
    graphStatus['h'].settings = hSettingsFromUI()
    settingsGraph('h');
    return false;
}

var hSettingsFromUI = function(){ 
    var hState = {};
    hState.revComplement = $('#revComplement').is(':checked')? true : false;
    hState.similarityPercent = Math.max(20,Math.min(100,$('#similarityPercent').val()));
    hState.sequences = [];
    $('.highlighterSequence').each(function(i){
        var validSequence = $(this).find('.sequenceInput').val().match(/([acgtn]+)/i)[0] || "AAAAAAAA";
        var validColor = $(this).find('.sequenceColor').val().match(/[0-9a-f]{6}/gi)[0] || "00ff00";
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
    $('#revComplement').prop('checked', hState.revComplement);
    try { $('#similarityPercent').simpleSlider("setValue", hState.similarityPercent); }
    catch (e) { $('#similarityPercent').val(hState.similarityPercent); }
    $('.highlighterSequence').remove()
    $.each(hState.sequences,function(i,v){
        var seq = $('#highlighterSequence').clone().removeAttr('id').addClass('highlighterSequence')
        seq.find('.showSeq').prop('checked', v.show);
        seq.find('.sequenceInput').val(v.sequence.toUpperCase());
        seq.find('.sequenceColor').val('#'+v.color)
        seq.insertBefore('.addSeq')
    })
}

var getCurrentPageURL = function(fullURL) {
    var graphString = ""
    for (var key in graphStatus) {
        if (graphStatus[key].visible == true) graphString += key;
    }
    var baseURL = (window.location.origin) ? window.location.origin : window.location.protocol + "//" + window.location.host;
    var currentURL = window.location.pathname
    var params = {"graphs":graphString,"start":state.start(),"scale":state.scale(),"width":state.width()};
    params.annotation = []
    $.each($.grep(annotationStatus, function(a){ return a.visible == true; }),function(i,v){
        params.annotation.push(v.FileName)
    })
    currentURL += "?" + $.param(params,true);
    if (graphStatus['h'].visible && graphStatus['h'].settings) currentURL += highlighterEncodeURL(graphStatus['h'].settings)
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
    if(annotation.End-annotation.Start < state.bpPerLine() && annotation.Start-state.start() >= 0) { //if selection is smaller than width
        popup.css({ 'left':(toPixels(graphStatus['n'].skixelOffset+Math.round((annotation.Start-state.start())/state.scale()).mod(state.width()))-150)+'px'})
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
    if (graphStatus[graph]) {
        g = graphStatus[graph]
        $('#showGraph-' + graph).prop('checked',false)
    }
    else if ($.grep(annotationStatus, function(a){ return a.FileName == graph; }).length > 0) {
        g = $.grep(annotationStatus, function(a){ return a.FileName == graph; })[0]
        graph = $.grep(annotationStatus, function(a){ return a.FileName == graph; })[0].FileName
        $('#showAnnotation-' + graph).prop('checked',false)
    }
    else return false
    g.visible = false;
    $('#graphLabel-' + graph).hide();
    isInvalidDisplay = true;
}
var showGraph = function(graph) {
    if (graphStatus[graph]) {
        g = graphStatus[graph]
        $('#showGraph-' + graph).prop('checked',true)
    }
    else if ($.grep(annotationStatus, function(a){ return a.FileName == graph; }).length > 0) {
        g = $.grep(annotationStatus, function(a){ return a.FileName == graph; })[0]
        graph = $.grep(annotationStatus, function(a){ return a.FileName == graph; })[0].FileName
        $('#showAnnotation-' + graph).prop('checked',true)
    }
    else return false
    g.visible = true;
    $('#graphLabel-' + graph).show();
    isInvalidDisplay = true;
}
var updateDials = function() {
    updateWidth();
    updateStart();
    // updateEnd();
    updateScale();

    if(activeAnnotation) {
        $('#annotationDetail').remove()
        showAnnotationDetail(annotations[activeAnnotation.FileName][activeAnnotation.annotation])
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
StateObject.prototype.chunkSize = 65536;
StateObject.prototype.width = getterSetter({defaultVal:100, filter:/(\d+)/, min:1, max:1024});
StateObject.prototype.start = getterSetter({defaultVal:1, filter:/(\d+)/, min:1, max:function(){return fileLength - (skixelsOnScreen - (25+37)*state.width())*state.scale()/2 + 1}})
StateObject.prototype.scale = getterSetter({defaultVal:1, filter:/(\d+)/, min:1, max:5000});
StateObject.prototype.end = function(a){ return state.start(a.match(/(\d+)/)[0] - ( skixelsOnScreen - (25+37)*state.width() - 1 )*state.scale())}
StateObject.prototype.goToEnd = function(){ return this.start(fileLength); }
StateObject.prototype.scaleToFile = function(){ 
    this.start(1);
    this.width(200);
    calcSkixelsOnScreen();
    var newScale = fileLength/(skixelsOnScreen-20*this.width());
    return newScale < 50 ? this.scale(newScale) : this.scale(round(newScale,50,"up"));
}
StateObject.prototype.chunkSizeBP = function(){ return this.chunkSize * this.scale(); }
StateObject.prototype.chunkStart = function(start){ 
    start = start || this.startTopOfScreen(1);
    return Math.floor(start/state.chunkSizeBP())*state.chunkSizeBP() + 1;
}
StateObject.prototype.bpPerLine = function(){ return this.width() * this.scale(); }
StateObject.prototype.startTopOfScreen = function(min1){ return min1? Math.max(1,state.start() - 8*state.bpPerLine()) : state.start() - 8*state.bpPerLine(); }
var double = function(a){ return a * 2; }
var half = function(a){ return a / 2; }
var lines = function(a){ return Math.round(a) * state.bpPerLine(); }
var by = function(d){ return function(a){ return a + d; }}
var random = function(a){ return Math.random()*fileLength;}

