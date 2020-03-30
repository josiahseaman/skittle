from django.shortcuts import render
from django.http import HttpResponse, HttpResponseRedirect
from django.core.mail import send_mail
from django.conf import settings
from django.contrib.auth import login, authenticate

from SkittleCore.models import SkittleUser
from SkittleCore.forms import UserCreationForm

def home(request):
    return render(request, 'home.html')

def learn(request):
    if request.user.is_authenticated():
        request.user.NewUser = False
        request.user.save()
    return HttpResponseRedirect('/browse/homo/sapiens/hg19/chrY/?graphs=bn&start=1468365&scale=1&width=105&annotation=gencode#learn')

def loggedInRedirect(request):
    if request.user.is_authenticated() and not request.user.NewUser:
        return HttpResponseRedirect('/discover/')
    else:
        return learn(request)

def createUser(request):
    if request.method == "POST":
        form = UserCreationForm(request.POST)
        if form.is_valid():
            newUser = SkittleUser.objects.create_user(**form.cleaned_data)

            user = authenticate(username=request.POST['email'],password=request.POST['password1'])
            login(request,user)
            return HttpResponseRedirect('/learn/')
    else:
        form = UserCreationForm()
    return render(request, 'createUser.html',{'form' : form})


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
            if sender_email:
                message += "from: " + sender_email + "\n"
            if contact_sender:
                message += "requested a reply.\n"
            if current_view:
                message += "url: " + current_view + ' \n'
            message += '\nMessage:\n' + content
            receivers = ["info@newline.us", ]
            if contact_sender and sender_email:
                receivers.append(sender_email)
            try:
                send_mail(subject, message, settings.DEFAULT_FROM_EMAIL, receivers, fail_silently=False)
                return HttpResponse("Success")
            except Exception as e:
                return HttpResponse("Something went wrong: " + str(e))
    else:
        return HttpResponse("Something went wrong.")


def google(request):
    return render(request, 'googlef44684f34c2340e5.html')