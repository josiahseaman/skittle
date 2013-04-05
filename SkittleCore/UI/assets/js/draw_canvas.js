

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
        }
    },50)
}

var imageRequestor = function(graph,chunkOffset) {
    imageObj[graph] = imageObj[graph] || []

    var graphPath = graphURL(graph,chunkOffset)

    if (!imageObj[graph][chunkOffset] 
        || ( imageObj[graph][chunkOffset].complete 
            && imageObj[graph][chunkOffset].source != graphPath ) ) {
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
    var startTopOfScreen = (state.start()-8*state.bpPerLine()) >  0 ? (state.start()-8*state.bpPerLine()) : 1
    var startChunk = ( ( Math.floor(startTopOfScreen/(65536*state.scale()) ) + chunkOffset )*65536*state.scale() + 1 );
    var graphPath = "data.png?graph=" + graph + "&start=" + startChunk + "&scale=" + state.scale();
    if (graphStatus[graph].rasterGraph != true) graphPath += "&width=" + expRound(state.width(),graphStatus[graph].widthTolerance)
    if (graph == 'h') graphPath += "&searchStart=" + selectionStart + "&searchStop=" + selectionEnd
    if (graphStatus[graph].colorPaletteSensitive) graphPath += "&colorPalette="+colorPalette
    return graphPath
}
    var loadedAnnotations = []
var annotationRequestor = function(chunkOffset) {
    if(!loadedAnnotations[chunkOffset] && chunkOffset <= fileLength) {
        $.getJSON('annotation.json',{start:chunkOffset},function(data){
            $.each(data,function(i,v){
                $.extend(annotations,v)
            })
            isInvalidDisplay = true
            loadedAnnotations[chunkOffset] = true
        }).error(function(jqXHR, textStatus, errorThrown){
            if(jqXHR.responseText=="None") loadedAnnotations[chunkOffset] = true
            console.log(chunkOffset,jqXHR.responseText,textStatus,errorThrown)
        })
    }
}


// the part that does the actual work
var gutterWidth = 8 //skixels
var minimumWidth = 160 //pixels
var calculateOffsetWidth = function(skixelWidthofGraph) {
    return Math.max( (skixelWidthofGraph + gutterWidth), toSkixels(minimumWidth) )
}

var drawGraphs = function() {
    drawPixelStuff = {'pre':[],'post':[]};
    b.clearRect(0,0,1024,1000)
    var offset = xOffset + gutterWidth
    var chunks = Math.min( Math.ceil(skixelsOnScreen/65536 + 1),(Math.ceil(fileLength/(65536*state.scale()))-Math.floor((state.start()-8*state.bpPerLine())/(65536*state.scale()))),Math.ceil(fileLength/(65536*state.scale())) )
    // for (key in graphStatus) {
    $.each(graphOrder,function(i,key){
        if (graphStatus[key] && graphStatus[key].visible) {
            graphStatus[key].skixelOffset = offset;
            var skixelWidthofGraph = graphStatus[key].skixelWidth = drawGraph(key,offset,chunks);
            skixelWidthofGraph = Math.max(skixelWidthofGraph,toSkixels(minimumWidth))
            offset = offset + skixelWidthofGraph;
            if (graphStatus[key].help) offset += toSkixels(200);
            $('#graphLabel-' + key).width(toPixels(skixelWidthofGraph));
        }
    })

    c.clearRect(0,0,2000,1000) // render on visible canvas (which has scale applied)
    while (drawPixelStuff['pre'].length) { // draw any pixel grid stuff
        var fn = drawPixelStuff['pre'].shift();
        fn();
    }
    c.drawImage(b.canvas, 0, 0);

    while (drawPixelStuff['post'].length) { // draw any pixel grid stuff
        var fn = drawPixelStuff['post'].shift();
        fn();
    }

}

var drawGraph = function(graph,offset,chunks) {
    switch (graph) {
        case "a": return drawAnnotations(offset,chunks)
        case "b": return drawNucBias(offset,chunks);
        case "m": return drawRMap(offset,chunks);
        case "s": return drawSimHeat(offset,chunks);
        case "r": return drawRepeatOverview(offset,chunks);
        default: 
            if (graphStatus[graph].rasterGraph == true) return drawRasterGraph(graph,offset,chunks);
            else return drawVerticalGraph(graph,offset,chunks);
    }
}
var drawRasterGraph = function(graph,offset,chunks) {
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

    var fadePercent = 1
    if (annotationSelectedStart > 0) {
        fadePercent = 0.35
    }
    var chunkStartOffset = Math.max( Math.floor((state.start()/state.scale()-state.width()*8)/(65536) ), 0 )*65536
    var startOffset = ( Math.round(state.start()/state.scale()) - 1 - state.width()*8 - chunkStartOffset )*4;
    var selectedStart = (((annotationSelectedStart-1)/state.scale()) - chunkStartOffset)*4;
    var selectedEnd = (((annotationSelectedEnd-1)/state.scale()) - chunkStartOffset)*4;
    for (var x = 0; x < newData.length; x += 4) { // read in data from original pixel by pixel
        var y = x + startOffset
        newData[x] = data[y] || 0;
        newData[x + 1] = data[y + 1] || 0;
        newData[x + 2] = data[y + 2] || 0;
        (selectedEnd >=y && selectedStart<=y) ? newData[x + 3] = data[y + 3] : newData[x + 3] = data[y + 3]*fadePercent;
    }
    b.putImageData(newImageData, offset, 0);

    return calculateOffsetWidth(state.width())
}
var drawVerticalGraph = function(graph,offset,chunks) {
    var graphWidth = 0, graphHeight = 0;
    var stretchFactor = expRound(state.width(),graphStatus[graph].widthTolerance)/state.width() 
    for (var i=0;i<chunks;i++) {
        var imageObj = imageRequestor(graph,i)
        if(!imageObj.complete || imageObj.naturalWidth === 0) imageObj = imageUnrendered;
        else var graphWidth = imageObj.width
        var vOffset = -Math.round(((Math.round(state.start()/state.scale())-8*state.width())%(65536))/(state.width()) - i*(65536/state.width()));
        (i == chunks - 1) ? graphHeight = imageObj.height*stretchFactor : graphHeight = Math.ceil(65536/state.width()) // don't stretch last chunk
        // graphHeight = Math.ceil(imageObj.height*stretchFactor)
        b.drawImage(imageObj,offset,vOffset,graphWidth,graphHeight) // render data on hidden canvas
    }
    return calculateOffsetWidth(graphWidth)
}
var drawAnnotations = function(offset,chunks) {
    var annotationWidth = 3
    var columnFilledTilRow = []


    for (var i = 0; i < chunks; i++) {
        annotationRequestor((Math.floor(state.start()/65536)+i)*65536+1)
    };

    visibleAnnotations = []
    
    $.each(annotations,function(i,annotation){ 
            annotation["End"] = annotation["End"] || annotation["Start"]
            if (   (annotation["Start"] < ( state.start() + (skixelsOnScreen + 37*state.width() - 1)*state.scale() ) && annotation["End"] > ( state.start() + (skixelsOnScreen + 37*state.width() - 1)*state.scale() ) )
                || (annotation["Start"] < (state.start() - 8*state.bpPerLine()) && annotation["End"] > (state.start() - 8*state.bpPerLine()) )
                || (annotation["Start"] > (state.start() - 8*state.bpPerLine()) && annotation["End"] < ( state.start() + (skixelsOnScreen + 37*state.width() - 1)*state.scale() ) ) ) {
                    // annotation[0] = annotation[0] || i
                    visibleAnnotations.push(i)
            }
    })
    visibleAnnotations.sort(function(a,b){return annotations[a]["Start"]-annotations[b]["Start"] + (annotations[b]["End"]/annotations[b]["Start"]-annotations[a]["End"]/annotations[a]["Start"])})

    $.each(visibleAnnotations,function(i,v){
        var currentColumn = 0
        var startRow = Math.floor((annotations[v]["Start"]-state.start())/(state.bpPerLine())+8)
        var rowHeight = Math.ceil((annotations[v]["End"]-annotations[v]["Start"])/(state.bpPerLine()))

        for (currentColumn=0;currentColumn<=columnFilledTilRow.length;currentColumn++) {
            if (!columnFilledTilRow[currentColumn] || startRow > columnFilledTilRow[currentColumn]) {
                columnFilledTilRow[currentColumn] = startRow + rowHeight+1
                annotations[v].column = currentColumn
                annotations[v].startRow = startRow
                annotations[v].rowHeight = rowHeight
                break;
            }
        }
    })
    
    var offsetWidth = calculateOffsetWidth(columnFilledTilRow.length*annotationWidth)
    drawPixelStuff['post'].push(function(){
        $.each(visibleAnnotations,function(i,v){
            if (annotations[v]["End"]-annotations[v]["Start"]>3) {
                c.beginPath()
                c.rect(offset - gutterWidth + offsetWidth-annotations[v].column*annotationWidth+1,annotations[v].startRow,-2/(zoom*3),annotations[v].rowHeight)
                annotations[v].color = annotations[v].color || getGoodDeterministicColor(annotations[v]["Start"] + "" + annotations[v]["End"] +"" + i + "")
                annotations[v].active == true ? c.fillStyle='#fff' : c.fillStyle=annotations[v].color
                c.fill()
            }
            else {
                c.beginPath()
                c.arc(offset - gutterWidth + offsetWidth-annotations[v].column*annotationWidth+0.7,annotations[v].startRow+0.5,annotationWidth/4,0,2*Math.PI,false)
                annotations[v].color = annotations[v].color || getGoodDeterministicColor(annotations[v]["Start"] + "" + annotations[v]["End"] + "" + i + "")
                annotations[v].active == true ? c.fillStyle='#fff' : c.fillStyle=annotations[v].color
                c.fill()
            }
        })

    })


    return offsetWidth
}
var drawNucBias = function(offset,chunks) {

    drawPixelStuff['pre'].push(function() { 
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
    })

    drawVerticalGraph("b",offset,chunks)
    return calculateOffsetWidth(60)
}
var drawRMap = function(offset,chunks) {
    var offsetWidth = drawVerticalGraph("m",offset,chunks)
    
    drawPixelStuff['post'].push(function() { 
        bpPerLine = state.bpPerLine()
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
            // var widthPosition = offset + 17.315*Math.log(state.bpPerLine()) - 42.85 - Math.min(0.9,(state.bpPerLine())/36);
            widthPosition = Math.round(widthPosition*3)/3
            c.beginPath();
            c.moveTo(widthPosition-0.18181818,0)
            c.lineTo(widthPosition-0.18181818,500)
            c.moveTo(widthPosition+1.18181818,0)
            c.lineTo(widthPosition+1.18181818,500)
            c.strokeStyle = "#f00"
            c.lineWidth = 0.333333333
            c.stroke();
        }
    })
    return Math.max(offsetWidth,calculateOffsetWidth(143))
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
    var bpPerLine = state.bpPerLine()
    var offsetStart = state.start() - bpPerLine*8
    var linesFromTop = offsetStart/bpPerLine
    var chunksFromTop = Math.max(Math.floor(offsetStart/(state.scale()*65536)),0)
    var bpFromLastChunk = offsetStart - chunksFromTop*(65536*state.scale())
    var linesFromLastChunk = Math.floor(bpFromLastChunk/bpPerLine)
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
        var vOffset = -Math.round(((state.start()-8*state.width())%65536)/(state.bpPerLine()));
        // b.putImageData(imageData, offset+320, vOffset);
    return calculateOffsetWidth(displayWidth)
    
}
var drawRepeatOverview = function(offset,chunks) {
    var offsetWidth = drawRasterGraph('r',offset+11,chunks)

    var height = toSkixels($('#canvasContainer').height()-70)
    drawPixelStuff['post'].push(function() { 
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
    })
    return offsetWidth+11
}
var generatePlaceholderImage = function() {

}
