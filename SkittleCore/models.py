'''
Created on Nov 30, 2012

@author: Josiah
'''
from django.db import models
from django.conf import settings
from django.contrib.auth.models import AbstractBaseUser, BaseUserManager, PermissionsMixin
from django.utils import timezone
from django.utils.http import urlquote
from django.core.mail import send_mail

from FastaFiles import readFile
import DNAStorage.StorageRequestHandler as StorageRequestHandler


chunkSize = settings.CHUNK_SIZE

'''
This is the single global state packet that defines a view state in Skittle.  
This state packet is equivalent to an URL or a request from the Skittle website.
'''
class basePacket(models.Model):
    specimen = models.CharField(max_length=200, default='hg18')
    chromosome = models.CharField(max_length=200, default='chrY-sample')
    '''It is debatable whether or not the sequence should be stored in the state
    variable since it is only referenced at the first level operation.  Past the first
    step, the input sequence is more likely to be a floatList produced by the previous
    operation.'''
    seq = models.TextField(default=None, null=True)
    colorPalette = models.CharField(max_length=50,
                                    choices=[("Spring", "Spring"),
                                             ("Summer", "Summer"),
                                             ("Dark", "Dark"),
                                             ("DRUMS", "DRUMS"),
                                             ("Blues", "Blues"),
                                             ("Reds", "Reds"),
                                             ('Classic', 'Classic')],
                                    default='Classic')
    width = models.IntegerField(default=None, null=True)
    scale = models.IntegerField(default=None, null=True)
    '''Internally, start begins at 0.  Biologists count indices starting at 1, so this number
    is 1 less than the number displayed on the website.  This also means that you should print
    index+1 whenever you are writing user readable output.'''
    start = models.IntegerField(default=None, null=True)
    length = models.IntegerField(default=0)
    requestedGraph = models.CharField(max_length=1, default=None, null=True)

    searchStart = models.IntegerField(default=1)
    searchStop = models.IntegerField(default=1)

    class Meta:
        abstract = True

class StatePacket(basePacket):
    # NOTE: We can store other state items that come up here.
    # Session management and stuff. This will be nice when the Social aspect comes up.
    userId = models.IntegerField(unique=True)

    class Meta:
        abstract = False

class RequestPacket(basePacket):
    def copy(self):
        c = RequestPacket()
        #copy everything except the sequence
        c.specimen = self.specimen
        c.chromosome = self.chromosome
        c.seq = ''
        c.colorPalette = self.colorPalette
        c.width = self.width
        c.scale = self.scale
        c.start = self.start
        c.length = self.length
        c.requestedGraph = self.requestedGraph
        c.searchStart = self.searchStart
        c.searchStop = self.searchStop
        return c

    def getFastaFilePath(self):
        return StorageRequestHandler.GetFastaFilePath(self.specimen, self.chromosome, self.start)

    '''Derived value height may need to be further reduced for functions that must scan ahead.'''

    def height(self):
        return self.length / self.nucleotidesPerLine()

    def nucleotidesPerLine(self):
        return self.width * self.scale

    '''This is a multifunctional 'make the file bigger' read logic for sequential chunks'''

    def readAndAppendNextChunk(self, addPadding=False):
        assert StorageRequestHandler.GetFastaFilePath(self.specimen, self.chromosome,
                                                      1) is not None, "Specimen and Chromosome is not in the database"
        startBackup = self.start
        if not self.seq:
            self.seq = '' #ensure that seq is at least a string object
        self.start = self.start + len(self.seq) # jump to the end of the current sequence  (+ chunkSize)

        #print "Requesting",self.specimen, self.chromosome, self.start
        sequence = readFile(self)# see if there's a file that begins where you end, this will stop on a partial file
        if sequence is not None:
            self.seq = self.seq + sequence #append two sequences together
        elif addPadding:
            self.seq = self.seq + ('N' * chunkSize)
        self.start = startBackup
        self.length = len(self.seq)
        return self

    def readFastaChunks(self):
        numChunks = self.scale or 1
        if self.seq is not None and len(self.seq) >= (
                (numChunks - 1) * chunkSize) + 1: #at least on character in the last chunk
            return
        self.seq = ''
        self.length = len(self.seq)
        partialSequences = [None] * numChunks
        for index, chunkStart in enumerate(range(self.start, self.start + numChunks * chunkSize, chunkSize)):
            tempState = self.copy()
            tempState.start = chunkStart
            partialSequences[index] = readFile(tempState)
            if partialSequences[index] is None:
                partialSequences[index] = ''
        self.seq = ''.join(partialSequences)
        if self.scale >= 10:
            print "Done reading files"
        self.length = len(self.seq)

    class Meta(basePacket.Meta):
        managed = False

class SkittleUserManager(BaseUserManager):
    def create_user(self, email=None, password1=None, FirstName=None, LastName=None, **extra_fields):
        if not email:
            raise ValueError("Users must have an email address")
        if not FirstName:
            raise ValueError("Users must have a first name")
        if not password1:
            raise ValueError("Users must set a password")

        email = SkittleUserManager.normalize_email(email)
        user = SkittleUser(email=email, FirstName=FirstName, LastName=LastName)

        user.set_password(password1)
        user.save()
        return user

    def create_superuser(self, email=None, password=None, FirstName="Admin", LastName=None, **extra_fields):
        if not email:
            raise ValueError("Users must have an email address")
        if not FirstName:
            raise ValueError("Users must have a first name")
        if not password:
            raise ValueError("Users must set a password")

        user = self.create_user(email, password, FirstName, LastName)
        user.IsAdmin = True
        user.is_superuser = True
        user.save()
        return user

class SkittleUser(AbstractBaseUser, PermissionsMixin):
    email = models.EmailField(verbose_name='Email Address', max_length=255, unique=True, db_index=True, db_column="Email", help_text='Your email address will be treated as a Username for this site.',)
    FirstName = models.CharField(verbose_name='First Name', max_length=255,)
    LastName = models.CharField(verbose_name='Last Name', max_length=255, null=True, blank=True,)
    IsAdmin = models.BooleanField(verbose_name='Admin Status', default=False, help_text='Designates whether this user is an Admin/On Staff or not.',)
    IsActive = models.BooleanField(verbose_name='Active Status', default=True,)
    DateJoined = models.DateTimeField(verbose_name='Date joined', default=timezone.now)
    NewUser = models.BooleanField(verbose_name='New user', default=True, help_text='Designates if this is the user\'s first visit to the site or not.')

    is_staff = IsAdmin
    is_active = IsActive

    objects = SkittleUserManager()

    USERNAME_FIELD = 'email'
    REQUIRED_FIELDS = []

    class Meta:
        verbose_name = "User"
        verbose_name_plural = "Users"

    def get_absolute_url(self):
        return "/users/%s/" % urlquote(self.email)

    def get_full_name(self):
        full_name = '%s %s' % (self.FirstName, self.LastName)
        return full_name.strip()

    def get_short_name(self):
        return self.FirstName

    def email_user(self, subject, message, from_email=None):
        send_mail(subject, message, from_email, [self.email])

    @property
    def is_staff(self):
        "Is the user a member of the admin team?"
        return self.IsAdmin

    @property
    def is_active(self):
        return self.IsActive

    State = models.OneToOneField(StatePacket, null=True)

class ProcessQueue(models.Model):
    Specimen = models.CharField(max_length=200)
    Chromosome = models.CharField(max_length=200)
    Graph = models.CharField(max_length=1)
    Start = models.IntegerField()
    Scale = models.IntegerField(default=None, null=True)
    CharsPerLine = models.IntegerField(default=None, null=True)
    