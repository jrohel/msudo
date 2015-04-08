# msudo
Linux/Unix command line utility to run programs as another user

msudo is a program for Unix-like computer operating systems that allows users to run programs with the security privileges of another user (normally the superuser, or root).

Allowed combinations of users and programs with arguments are readed from the /etc/msudo.conf file. Arguments of run programs can be defined as regular expression.

I wrote this small utility for Linux embeded systems. I tried use another existing utility sudo. But it was too big and not offer what I exactly needed.
