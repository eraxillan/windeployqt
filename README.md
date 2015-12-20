# The Windows Deployment Tool
This is the Qt5 deployment utility for Windows fork.  
Fork purpose: make an embeddable version of utility,  
for using for example in CMake build system.  
"Embeddable" means at least  
"not dependent from external environment stuff like PATH".  
And some refactoring will be done.  
Spaghetti code is evil :)  

# [Original description](http://doc.qt.io/qt-5/windows-deployment.html)

The Windows deployment tool can be found in `QTDIR/bin/windeployqt`.  
It is designed to automate the process of creating a deployable folder  
that contains all libraries, QML imports, plugins, translations  
that are required to run the application from that folder.  
This is used to create the sandbox for Windows Runtime  
or an installation tree for Windows desktop applications  
that can be easily bundled by an installer.  
