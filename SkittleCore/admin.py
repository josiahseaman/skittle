from django import forms
from django.contrib import admin
from django.contrib.auth.models import Group
from django.contrib.auth.admin import UserAdmin
from django.contrib.auth.forms import ReadOnlyPasswordHashField

from SkittleCore.models import SkittleUser

class UserCreationForm(forms.ModelForm):
    password1 = forms.CharField(label='Password', widget=forms.PasswordInput)
    password2 = forms.CharField(label='Password confirmation', widget=forms.PasswordInput)

    class Meta:
        model = SkittleUser
        fields = ('email', 'FirstName', 'LastName')

    def clean_password2(self):
        password1 = self.cleaned_data.get("password1")
        password2 = self.cleaned_data.get("password2")
        if password1 and password2 and password1 != password2:
            raise forms.ValidationError("Passwords don't match")
        return password2

    def save(self, commit=True):
        user = super(UserCreationForm, self).save(commit=False)
        user.set_password(self.cleaned_data["password1"])
        if commit:
            user.save()
        return user

class UserChangeForm(forms.ModelForm):
    password = ReadOnlyPasswordHashField()

    class Meta:
        model = SkittleUser
        fields = ['email', 'FirstName', 'LastName', 'password', 'IsActive', 'IsAdmin']

    def clean_password(self):
        return self.initial["password"]

class SkittleUserAdmin(UserAdmin):
    form = UserChangeForm
    add_form = UserCreationForm

    list_display = ('email', 'IsAdmin')
    list_filter = ('IsAdmin',)
    fieldsets = (
        (None, {'fields': ('email', 'password')}),
        ('Personal info', {'fields': ('FirstName', 'LastName',)}),
        ('Permissions', {'fields': ('IsAdmin',)}),
    )
    add_fieldsets = (
        (None, {'classes': ('wide',), 'fields': ('email', 'FirstName', 'LastName', 'password1', 'password2')}),
    )
    search_fields = ('email', 'FirstName', 'LastName')
    ordering = ('email', 'FirstName', 'LastName')
    filter_horizontal = ()

admin.site.register(SkittleUser, SkittleUserAdmin)
admin.site.unregister(Group)