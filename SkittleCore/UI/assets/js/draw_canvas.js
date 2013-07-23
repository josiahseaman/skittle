var init = function() {
    imageObj = {};

    c.imageSmoothingEnabled = false; // so it won't be blury when it scales
    c.webkitImageSmoothingEnabled = false;
    c.mozImageSmoothingEnabled = false;
    c.scale(Math.round(3*zoom),Math.round(3*zoom))

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
    $.each(graphStatus,function(i,v){ //add unordered graphs to the end of the order
        if ($.inArray(i,graphOrder)<0) {
            graphOrder.push(i)
        }
    })
    for (var i=0;i<graphOrder.length;i++) {
        $("#graphLabel-" + graphOrder[i]).appendTo("#graph-labels ul")
        $("#showGraph-" + graphOrder[i]).parent().appendTo("#graphList ul")
    }

    var loopCount = 0
    var updateURL = true
    var mainLoop = window.setInterval(function(){
        if(isInvalidDisplay) {
            isInvalidDisplay = false
            loopCount = 0
            drawGraphs();
            updateDials();
        }
        if (updateURL && loopCount++ == 100) {
            window.history.pushState(null,null,getCurrentPageURL())
            simultainiousRequests = 0; //reset in case something weird happened
        }
    },50)
}

var imageRequestor = function(graph,chunkOffset) {
    if (chunkOffset > fileLength) return false;

    imageObj[graph] = imageObj[graph] || []

    var graphPath = graphURL(graph,chunkOffset)

    if (!imageObj[graph][chunkOffset] 
        || ( imageObj[graph][chunkOffset].complete && imageObj[graph][chunkOffset].source != graphPath ) ) {
        imageObj[graph][chunkOffset] = new Image();
        imageObj[graph][chunkOffset].source = graphPath;
        imageObj[graph][chunkOffset].src = graphPath;
        imageObj[graph][chunkOffset].onload = function() { // causes a little bit of jitter when scrolling
            isInvalidDisplay = true
        }
        imageObj[graph][chunkOffset].onerror = function() {
            console.log('not a valid filename')
        }
    }
    return imageObj[graph][chunkOffset]
}
var graphURL = function(graph,chunkOffset) {
    var startChunk = state.chunkStart() + chunkOffset*state.chunkSizeBP();
    var graphPath = "data.png?graph=" + graph + "&start=" + startChunk + "&scale=" + state.scale();
    if (graphStatus[graph].rasterGraph != true) graphPath += "&width=" + expRound(state.width(),graphStatus[graph].widthTolerance)
    if (graph == 'h' && graphStatus['h'].settings) graphPath += highlighterEncodeURL(graphStatus['h'].settings)
    if (graphStatus[graph].colorPaletteSensitive) graphPath += "&colorPalette="+colorPalette
    return graphPath

}
    var loadedAnnotations = []
var annotationRequestor = function(chunkOffset,file) {
    if (chunkOffset > fileLength) return false;

    if(!loadedAnnotations[chunkOffset] && chunkOffset <= fileLength && simultainiousRequests < 4) {
        simultainiousRequests++

        var activeAnnotations = [file];

        $.getJSON('annotation.json',{start:chunkOffset,annotation:activeAnnotations},function(data){
            $.each(data,function(i,v){
                annotations[i] = annotations[i] || {}
                $.extend(annotations[i],v)
                simultainiousRequests--
            })
            isInvalidDisplay = true
            loadedAnnotations[chunkOffset] = true
        }).error(function(jqXHR, textStatus, errorThrown){
            if(jqXHR.responseText=="None") loadedAnnotations[chunkOffset] = true
            console.log(chunkOffset,jqXHR.responseText,textStatus,errorThrown)
            simultainiousRequests--
        })
    }
}


// drawing functions
var gutterWidth = 8 //skixels
var minimumWidth = 160 //pixels
var calculateOffsetWidth = function(skixelWidthofGraph) {
    return Math.max( (skixelWidthofGraph + gutterWidth), toSkixels(minimumWidth) )
}

var drawGraphs = function() {
    b.clearRect(0,0,1024,1000)
    var offset = xOffset + gutterWidth;

    var chunksOnScreenPlus1 = Math.ceil(skixelsOnScreen/65536 + 1)
    var chunksInFile = Math.ceil(fileLength/(state.chunkSizeBP()))
    var chunksRemainingInFile = chunksInFile - Math.floor( state.startTopOfScreen()/state.chunkSizeBP() )
    var chunks = Math.min( chunksOnScreenPlus1, chunksRemainingInFile, chunksInFile )

    $.each(annotationStatus,function(i,v){
        if (v.visible) {
            v.skixelOffset = offset
            var skixelWidthofGraph = v.skixelWidth = drawAnnotation(v.FileName,i,offset,chunks);
            skixelWidthofGraph = Math.max(skixelWidthofGraph,toSkixels(minimumWidth))
            offset = offset + skixelWidthofGraph;
            $('#graphLabel-' + v.FileName).width(toPixels(skixelWidthofGraph));
        }
    })

    $.each(graphOrder,function(i,key){
        if (graphStatus[key] && graphStatus[key].visible) {
            graphStatus[key].skixelOffset = offset;
            graphStatus[key].skixelWidth = drawGraph(key,offset,chunks);
            skixelWidthofGraph = calculateOffsetWidth(graphStatus[key].skixelWidth)
            offset = offset + skixelWidthofGraph;
            if (graphStatus[key].help) offset += toSkixels(200);
            if (graphStatus[key].controls) offset += toSkixels(325);
            $('#graphLabel-' + key).width(toPixels(skixelWidthofGraph));
        }
    })

    c.clearRect(0,0,2000,1000) // render on visible canvas (which has scale applied)

    $.each(annotationStatus,function(i,v){
        if(v.visible && v.drawPixelPre) v.drawPixelPre()
    })
    $.each(graphOrder,function(i,key){
        if(graphStatus[key].visible && graphStatus[key].drawPixelPre) graphStatus[key].drawPixelPre()
    })

    c.drawImage(b.canvas, 0, 0);

    $.each(graphOrder,function(i,key){
        if(graphStatus[key].visible && graphStatus[key].drawPixelPost) graphStatus[key].drawPixelPost()
    })

}

var drawGraph = function(graph,offset,chunks) {
    switch (graph) {
        case "s": return drawSimHeat(offset,chunks);
        default: 
            if (graphStatus[graph].rasterGraph == true) return drawRasterGraph(graph,offset,chunks);
            else return drawVerticalGraph(graph,offset,chunks);
    }
}
var drawRasterGraph = function(graph,offset,chunks) {
    if (graphStatus[graph].leftOffset) offset += graphStatus[graph].leftOffset
    a.clearRect(0,0,1024,500)
    for (var i=0;i<chunks;i++) {
        var imageObj = imageRequestor(graph,i)
        if(!imageObj.complete || imageObj.naturalWidth === 0) imageObj = imageUnrendered;
        a.drawImage(imageObj,0,64*i) // render data on hidden canvas
    }

    var imageData = a.getImageData(0, 0, 1024, chunks*64);
    var data = imageData.data;
    var newImageData = b.createImageData(state.width(),toSkixels(1000)) //create new image data with desired dimentions (width)
    var newData = newImageData.data;

    var fadePercent = (annotationSelectedStart > 0) ? 0.5 : 1;
    
    var chunkStartOffset = round(state.startTopOfScreen(1), state.chunkSizeBP(), "down");
    var startOffset = Math.round( (state.startTopOfScreen() - chunkStartOffset - 1)/state.scale() )*4;
    var selectedStart = Math.round( (annotationSelectedStart - chunkStartOffset - 1)/state.scale() )*4;
    var selectedEnd = Math.round( (annotationSelectedEnd - chunkStartOffset - 1)/state.scale() )*4;

    for (var x = 0; x < newData.length; x += 4) { // read in data from original pixel by pixel
        var y = x + startOffset
        if (!(annotationSelectedStart > 0) || selectedEnd >= y && selectedStart <= y ) {
            newData[x] = data[y] || 0;
            newData[x + 1] = data[y + 1] || 0;
            newData[x + 2] = data[y + 2] || 0;
            newData[x + 3] = data[y + 3] || 0;
        } else {
            newData[x] = Math.min(data[y] + 80, 255);
            newData[x + 1] = Math.min(data[y + 1] + 80, 255);
            newData[x + 2] = Math.min(data[y + 2] + 80, 255);
            newData[x + 3] = data[y + 3]*fadePercent;
        }

    }
    b.putImageData(newImageData, offset, 0);

    graphWidth = state.width()
    if (graphStatus[graph].leftOffset) graphWidth += graphStatus[graph].leftOffset
    return graphWidth
}
var drawVerticalGraph = function(graph,offset,chunks) {
    var graphWidth = 0, graphHeight = 0;
    var stretchFactor = 1
    if (graphStatus[graph].stretchy == true) {
        stretchFactor = expRound(state.width(),graphStatus[graph].widthTolerance)/state.width();
    }
    for (var i=0;i<chunks;i++) {
        var imageObj = imageRequestor(graph,i)
        if(!imageObj.complete || imageObj.naturalWidth === 0) imageObj = imageUnrendered;
        else var graphWidth = imageObj.width
        var vOffset = -Math.round( state.startTopOfScreen() % state.chunkSizeBP() / state.bpPerLine() - i*65536/state.width() );
        graphHeight = Math.ceil(65536/state.width());
        if (i == chunks - 1 || !graphStatus[graph].stretchy) graphHeight = imageObj.height*stretchFactor;// don't stretch last chunk
        // graphHeight = Math.ceil(imageObj.height*stretchFactor)
        b.drawImage(imageObj,offset,vOffset,graphWidth,graphHeight) // render data on hidden canvas
    }
    if (graphStatus[graph].leftOffset) graphWidth += graphStatus[graph].leftOffset
    graphWidth = graphStatus[graph].skixelWidth? Math.max(graphStatus[graph].skixelWidth,graphWidth) : graphWidth;
    return graphWidth
}

var drawAnnotation = function(file,statusIndex,offset) {
    var annotationWidth = 3
    var columnFilledTilRow = []

    var chunks = skixelsOnScreen*state.scale()/state.chunkSize + 1; 

    if (chunks > 12) {
        hideGraph(file);
        return 0;
    }

    for (var i = 0; i < chunks; i++) {
        annotationRequestor((Math.floor(state.start()/65536)+i)*65536+1)
    };

    visibleAnnotations[file] = [];
    annotations[file] = annotations[file] || {}
    
    $.each(annotations[file],function(i,annotation){ //filter annotations that are on screen.
            annotation.End = annotation.End || annotation.Start
            var startBottomOfScreen = state.startTopOfScreen() + skixelsOnScreen*state.scale()
            var startTopOfScreen = state.startTopOfScreen() - 30*state.bpPerLine()
            if (   (annotation.Start < startBottomOfScreen && annotation.End > startBottomOfScreen )
                || (annotation.Start < startTopOfScreen    && annotation.End > startTopOfScreen )
                || (annotation.Start > startTopOfScreen    && annotation.End < startBottomOfScreen ) ) {
                    visibleAnnotations[file].push(i)
            }
    })
    visibleAnnotations[file].sort(function(a,b){
        var a = annotations[file][a]
        var b = annotations[file][b]
        return a.Start - b.Start + ((b.End-b.Start) - (a.End-a.Start))/10
    })

    $.each(visibleAnnotations[file],function(i,v){
        var a = annotations[file][v]
        var currentColumn = 0
        var startRow = Math.floor((a.Start-state.start())/(state.bpPerLine())+8)
        var rowHeight = Math.ceil((a.End-a.Start)/(state.bpPerLine()))

        for (currentColumn=0;currentColumn<=columnFilledTilRow.length;currentColumn++) {
            if (!columnFilledTilRow[currentColumn] || startRow > columnFilledTilRow[currentColumn]) {
                columnFilledTilRow[currentColumn] = startRow + rowHeight+1
                a.column = currentColumn
                a.startRow = startRow
                a.rowHeight = rowHeight
                break;
            }
        }
    })
    
    var offsetWidth = calculateOffsetWidth(columnFilledTilRow.length*annotationWidth)
    annotationStatus[statusIndex].drawPixelPre = function(){
        $.each(visibleAnnotations[file],function(i,v){
            var a = annotations[file][v]
            if (a.End - a.Start > 3) {
                c.beginPath()
                c.rect(offset - gutterWidth + offsetWidth-a.column*annotationWidth+1,a.startRow,-2/(zoom*3),a.rowHeight)
                a.color = a.color || getGoodDeterministicColor(a.Start + "" + a.End +"" + i + "")
                a.active == true ? c.fillStyle='#fff' : c.fillStyle=a.color
                c.fill()
            }
            else {
                c.beginPath()
                c.arc(offset - gutterWidth + offsetWidth-a.column*annotationWidth+0.7,a.startRow+0.5,annotationWidth/4,0,2*Math.PI,false)
                a.color = a.color || getGoodDeterministicColor(a.Start + "" + a.End + "" + i + "")
                a.active == true ? c.fillStyle='#fff' : c.fillStyle=a.color
                c.fill()
            }
        })
    }

    return offsetWidth
}


graphStatus['b'].drawPixelPre = function() { 
    var offset = graphStatus['b'].skixelOffset
    c.shadowColor   = 'rgba(0, 0, 0, 0)'; // 'cause when repeat overview sets shadow color to (0,0,0,1) it breaks nuc bias

    c.beginPath()
    c.rect(offset,0,60,500)
    c.fillStyle="#333";
    c.fill();
    c.beginPath()
    c.moveTo(offset+15.16161,0)
    c.lineTo(offset+15.16161,500)
    c.moveTo(offset+25.16161,0)
    c.lineTo(offset+25.16161,500)
    c.moveTo(offset+35.16161,0)
    c.lineTo(offset+35.16161,500)
    c.moveTo(offset+45.16161,0)
    c.lineTo(offset+45.16161,500)
    c.strokeStyle = '#888'
    c.lineWidth = 0.33333
    c.stroke()
    c.beginPath()
    c.moveTo(offset+5.16161,0)
    c.lineTo(offset+5.16161,500)
    c.moveTo(offset+10.16161,0)
    c.lineTo(offset+10.16161,500)
    c.moveTo(offset+30.16161,0)
    c.lineTo(offset+30.16161,500)
    c.moveTo(offset+50.16161,0)
    c.lineTo(offset+50.16161,500)
    c.moveTo(offset+55.16161,0)
    c.lineTo(offset+55.16161,500)
    c.strokeStyle = '#555'
    c.lineWidth = 0.33333
    c.stroke()
    c.beginPath()
    c.moveTo(offset+20,0)
    c.lineTo(offset+20,500)
    c.moveTo(offset+40,0)
    c.lineTo(offset+40,500)
    c.strokeStyle = '#000'
    c.lineWidth = 0.33333
    c.stroke()
}


graphStatus['m'].drawPixelPost = function() { 
    var offset = graphStatus['m'].skixelOffset

    var bpPerLine = state.bpPerLine()
    if ( bpPerLine >= 1 && bpPerLine < 26000 ) { //draw the red lines
        var cumulativeWidth = 0, megaColumn=0, subColumn=0;

        while (cumulativeWidth<(bpPerLine-12)) {
            cumulativeWidth += Math.pow(2,megaColumn)
            subColumn++
            if(subColumn>=12) {
                subColumn=0
                megaColumn++
            } 
        }
        var widthPosition = offset + 11 + megaColumn*12+subColumn -(cumulativeWidth-bpPerLine+12)/Math.pow(2,megaColumn)
        widthPosition = round(widthPosition,(1/3))

        c.beginPath();
        c.moveTo(widthPosition-0.18181818,0)
        c.lineTo(widthPosition-0.18181818,500)
        c.moveTo(widthPosition+1.18181818,0)
        c.lineTo(widthPosition+1.18181818,500)
        c.strokeStyle = "#f00"
        c.lineWidth = 0.333333333
        c.stroke();
    }
}

var drawSimHeat = function(offset,chunks) {
    a.clearRect(0,0,350,10000)
    var displayWidth = 300
    var stretchFactor = expRound(state.width(),graphStatus['s'].widthTolerance)/state.width()
    var lineHeight = Math.round(65536/state.width()) //Math.round((Math.round(state.width()/10)*10)/state.width()*Math.ceil(65536/state.width()));
    var displayWidth = Math.round(stretchFactor*displayWidth)
    for (var i=0;i<chunks;i++) {
        var imageObj = imageRequestor("s",i)
        if(!imageObj.complete || imageObj.naturalWidth === 0) imageObj = imageUnrendered;
        a.drawImage(imageObj,0,lineHeight*i,displayWidth,lineHeight) // render data on hidden canvas
        // a.beginPath();
        // a.moveTo(0,lineHeight*i+0.5)
        // a.lineTo(300,lineHeight*i+0.5)
        // a.strokeStyle = "#0f0"
        // a.stroke();
    }
    var imageData = a.getImageData(0, 0, displayWidth, chunks*lineHeight);
    var data = imageData.data;
    var newImageData = b.createImageData(displayWidth,displayWidth) //create new image data with desired dimentions (width)
    var newData = newImageData.data;

    var lineLength = displayWidth*4;
    var linesFromTop = state.startTopOfScreen()/state.bpPerLine()
    var chunksFromTop = Math.max(Math.floor(state.startTopOfScreen()/state.chunkSizeBP()),0)
    var bpFromLastChunk = state.startTopOfScreen() - chunksFromTop*state.chunkSizeBP()
    var linesFromLastChunk = Math.floor(bpFromLastChunk/state.bpPerLine())
    var startOffset = linesFromLastChunk*lineLength

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
        // var vOffset = -Math.round(((state.start()-8*state.width())%65536)/(state.bpPerLine()));
        // b.putImageData(imageData, offset+320, vOffset);
    return displayWidth
    
}


graphStatus['r'].leftOffset = 11;
graphStatus['r'].drawPixelPost = function() { 
    var offset = graphStatus['r'].skixelOffset
    var height = toSkixels($('#canvasContainer').height()-70)
    var legendGradient = c.createLinearGradient(0,0,0,height)
    legendGradient.addColorStop(0,'#00a')
    legendGradient.addColorStop(0.25,'#a00')
    legendGradient.addColorStop(0.5,'#aa0')
    legendGradient.addColorStop(0.75,'#0a0')
    legendGradient.addColorStop(1,'#0aa')
    c.fillStyle = legendGradient
    c.fillRect(offset,10,10,height)


    c.textBaseline = "middle"
    c.textAlign = "center"
    c.fillStyle = '#fff'
    c.font = "5px Exo,sans-serif"
    c.shadowOffsetX = 1;
    c.shadowOffsetY = 1;
    c.shadowBlur    = 2;
    c.shadowColor   = 'rgba(0, 0, 0, 1)';

    c.fillText("1bp",offset+5,13)
    c.fillText("36",offset+5,10+height/5*1)
    c.fillText("180",offset+5,10+height/5*2)
    c.fillText("1k",offset+5,10+height/5*3)
    c.fillText("5k",offset+5,10+height/5*4)
    c.fillText("26k",offset+5,10+height-2)
}


var generatePlaceholderImage = function() {

}
