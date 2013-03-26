// contactform.js	Version 0.9
// Requires jQuery 1.4.3
// by Marshall Smith

var contactForm = function() {
	var isError="false";
	var form = $('#bug-report-form')
	var submitButton = $('#bug-report-form button[type="submit"]')
	
	var sending = $(jQuery('<div id="sending" style="display:none; color:#ccdc33; line-height:1.5;">Sending...</div>'));
	var success = $(jQuery('<div id="success" style="display: none; height:94px;padding:35px 0; font-size:150%;text-align:center; color:#99B666;">Thank you for your feedback.</div>'));
	var fail = $(jQuery('<div id="fail" style="display:none; color:#dd3333; line-height:1.4;">Please fill in the shaded fields</div>'));
	form.append(sending).append(fail);

	form.submit(function(e) {
		e.preventDefault();
		submitButton.hide()
		fail.hide();
		sending.show();
		if(isError=="true") {
			e.preventDefault();
			sending.hide();
			submitButton.show()
			fail.show();
			
			isError="false";
		} else {
			//submit form via AJAX!
			$.post($(this).attr('action'), form.serialize(), function(data){
				if (data=="Success") {
					form.after(success);
					form.hide();
					sending.hide();
					success.show();
				}
				else {
					fail.html(data).show();
					submitButton.show()
					sending.hide();
				}
			});
		};
	});

}
