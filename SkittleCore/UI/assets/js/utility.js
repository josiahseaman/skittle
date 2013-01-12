Number.prototype.mod = function(n) {
	return ((this%n)+n)%n;
}
var toSkixels = function(pixels) {
    return Math.round(pixels/(3*zoom));
}
var toPixels = function(skixels) {
    return Math.round(skixels*3*zoom);
}