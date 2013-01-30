Number.prototype.mod = function(n) {
	return ((this%n)+n)%n;
}
var toSkixels = function(pixels) {
    return Math.round(pixels/(3*zoom));
}
var toPixels = function(skixels) {
    return Math.round(skixels*3*zoom);
}
var calcSkixelsOnScreen = function() {
    skixelsOnScreen = toSkixels($('#canvasContainer').height())*width;
}
var calcDeltaFromScrollEvent = function(e) {
	var delta = 0
    if (e.wheelDelta) { /* IE/Opera. */
            delta = e.wheelDelta/120;
    } else if (e.detail) { /** Mozilla case. */
            delta = -e.detail/3;// In Mozilla, sign of delta is different than in IE. Also, delta is multiple of 3.
    }
    return delta;
}