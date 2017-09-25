import os, shutil, glob

# Define current QtRoot and version
QtRoot = "/Users/dtrimarchi/third_party/Qt/5.9.1/clang_64"
QtVersion = "5.9.1"

# Loop on the pc files present in the folder
os.chdir("templates")
for fileName in glob.glob("*.pc"): 
    with open (fileName, 'r') as file: 
        fileData= file.read()
    fileData = fileData.replace("$QTROOT$",QtRoot)
    fileData = fileData.replace("$QTVERSION$",QtVersion)
    # mv file to templates
    with open (os.path.join("..",fileName), 'w') as file: 
        file.write(fileData)
