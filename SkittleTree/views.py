from django.shortcuts import render_to_response,render
from django.conf import settings 
from django.http import HttpResponseRedirect, HttpResponse, HttpRequest, QueryDict
from django.core.mail import EmailMessage

def home(request):
    return render(request, 'home.html')

def feedbackSend(request):
    if request.is_ajax() or request.method == 'POST':
        feedback_type = request.POST.get('feedback_type', 'Comment')
        sender_email = request.POST.get('feedback_sender_email', False)
        contact_sender = request.POST.get('feedback_contact_sender', False)
        content = request.POST.get('feedback_content', '')
        current_view = request.POST.get('feedback_current_view', '')
        if len(current_view) > 0:
            subject = 'Skittle Feedback:' + feedback_type
            message = ""
            if sender_email: message += "from: " + sender_email + "\n"
            if contact_sender: message += "requested a reply.\n"
            if current_view: message += "url: " + current_view + ' \n'
            message += '\nMessage:\n' + content
            email = EmailMessage(
                subject,
                message,
                'feedbackform@dnaskittle.com',
                ['admin@newlinetechnicalinnovations.com'],
                # headers = {'Reply-To': contact_sender}
                )
            try:
                email.send(fail_silently=False)
                return HttpResponse("Success")
            except Exception as e:
                return HttpResponse("Something went wrong: " + str(e))
    else:
        return HttpResponse("Something went wrong.")

def google(request):
    return render(request, 'googlef44684f34c2340e5.html')