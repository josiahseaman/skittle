var levels = ['Kingdom','Class','Genus','Species'];



var getTypeaheadValues = function(level) {

	var levelList = taxonomy
	for (var i=0;i<level;i++) {
		levelList = levelList[$('#taxonomyBuilder #level'+i).text()]
		if (!levelList) return []
	}
	if (!levelList) return []
	var list = []
	$.each(levelList,function(i,v){
		list.push(i)
	})
	return list
}

var addLevel = function(level) {
	levelInput = addLevelInput(level);
	levelInput
		.select()
		.autocomplete({source:getTypeaheadValues(level),minLength: 0})
		.blur(function() {
			if ($(this).val().length>0) {
				for (i=level+1;i<4;i++) $('#'+levels[i]).val(''); //set corresponding hidden inputs and clear down level ones
				$('#'+levels[level]).val($(this).val()).trigger('change');

				var levelSpan = $('<span id="level'+level+'" class="taxonomy">'+$(this).val()+'</span>');
				levelSpan
					.insertAfter($(this))
					.nextAll().remove()

				$(this).remove()
				level++
				if (level < 4) addLevel(level);
			}
		})
		.on('autocompleteselect', function(event, ui) { $(this).val(ui.item.label).trigger('blur'); }) // when you click on the list it should progress you to the next thing
}


var addLevelInput = function(level) {
	var value = ($('#taxonomyBuilder #level'+level).length>0) ? $('#taxonomyBuilder #level'+level).text() : "";
	var levelInput = $('<input type="text" id="level'+level+'" placeholder="' + levels[level] + '" value="'+value+'">')
	var insertPoint = $('#taxonomyBuilder #level'+level)
	if (insertPoint.length>0) {
		levelInput.insertAfter(insertPoint)
		insertPoint.remove()
	}
	else {
		$('#taxonomyBuilder').append(levelInput)
	}
	return levelInput;
}