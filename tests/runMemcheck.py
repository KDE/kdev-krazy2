#!/usr/bin/python
# -*- coding: utf-8 -*-
# run valgrind's memory error checker on all tests.
# filter uninteresting errors and known false positives
# eg staticly initialized memory from libraries like libfontconfig
#


# Modified from Coverage plugin tests for KDevelop 4
# http://websvn.kde.org/trunk/KDE/kdevelop/tools/coverage/tests/runMemcheck.py?revision=926694&view=markup
# which is licensed under GPL 2 or later
# http://websvn.kde.org/trunk/KDE/kdevelop/tools/coverage/coverageplugin.cpp?revision=926694&view=markup


from os import system, remove, path
from sys import exit, stdout
from subprocess import Popen, PIPE
from xml.dom.minidom import parse, parseString

def garbage(line):
    ''' filter for valgridn output'''
    return not line.startswith('<unknown program name>') and \
           not line.startswith('profiling:') and \
           line.find('</valgrindoutput>') # problem is that valgrind erroneously puts multiple of these end-document entries if processes are spawned _inside_ the exe under investigation

def memcheck(test):
    ''' run valgrind-memcheck on test in testdir. return xml output as string '''
    #proc = Popen("valgrind --tool=memcheck --leak-check=full --xml=yes " + test, stdout=PIPE, stderr=PIPE, shell=True, executable="/bin/bash")
    #proc.wait()
    #out = proc.stderr.readlines()
    system("valgrind --tool=memcheck --leak-check=full --xml=yes --xml-file=.memcheck.tmp --num-callers=50 " + test + " 1>/dev/null")
    out = open(".memcheck.tmp").readlines()
    remove(".memcheck.tmp")
    out = filter(garbage, out)
    return ''.join(out) + "\n</valgrindoutput>\n"

def xml_child_data(dom,tag):
    ''' extract child data for tag. return None if not found'''
    elem = dom.getElementsByTagName(tag)
    val = None
    if len(elem) != 0:
        val = elem[0].firstChild.data
    return val

class Frame:
    ''' single entry in a memory error backtrace '''
    def __init__(self, dom_frame):
        '''<frame>
        <ip>0x62ACDBF</ip>
        <obj>/home/nix/KdeDev/kde4/lib/libkdevplatformlanguage.so.1.0.0</obj>
        <fn>KDevelop::ParamIterator::ParamIterator(QString, QString, int)</fn>
        <dir>/home/nix/KdeDev/kdevplatform/language/duchain</dir>
        <file>stringhelpers.cpp</file>
        <line>292</line>
        </frame>'''
        self.obj   = xml_child_data(dom_frame, 'obj')
        self.func  = xml_child_data(dom_frame, 'fn')
        self.sfile = xml_child_data(dom_frame, 'file')
        self.sline = xml_child_data(dom_frame, 'line')

    def __str__(self):
        out = ""
        if self.func:
            out += "\t" + self.func
        if self.sfile and self.sline:
            out += " (" + self.sfile + ":" + self.sline + ")"
        #if self.obj:
            #out += "\t" + self.obj + "\n"
        out += "\n"
        return out

class BackTrace:
    ''' valgrind memcheck stack trace '''
    def __init__(self, errordom):
        self.dom = errordom
        self.kind = self.dom.getElementsByTagName('kind')[0].firstChild.data
        stack = self.dom.getElementsByTagName('frame')
        self.stack = []
        for frame in stack:
            if xml_child_data(frame, 'fn'): # filter anonymous frames out
                self.stack.append(Frame(frame))
        self.what = xml_child_data(self.dom, 'what')
        if self.dom.getElementsByTagName('xwhat').length > 0:
            self.what = xml_child_data(self.dom.getElementsByTagName('xwhat')[0], 'text')

    def is_definitely_lost(self):
        return self.kind == u'Leak_DefinitelyLost'

    def is_test(self, test_class_name):
        is_interesting = False
        for frame in self.stack:
            if frame.func:
                if frame.func.lower().find(test_class_name.lower()) != -1:
                    is_interesting = True # the trace contains the test class of the executable
                if frame.func.find('KDevelop::TestCore::initialize') != -1:
                    return False
                if frame.func.find('XcursorXcFileLoadImages') != -1:
                    return False # something deep in X server, not interested in this
                if frame.func.find('XRegisterIMInstantiateCallback') != -1:
                    return False # X-related static memory allocation, no leak
                if frame.func.find('FcDefaultSubstitute') != -1:
                    return False # something Qt-Font related, not interested in this
                if frame.func.find('__nss_database_lookup') != -1:
                    return False # more crap
        return is_interesting

    def __str__(self):
        out = self.what + "\n"
        for frame in self.stack:
            out += str(frame)
        return out

def parse_errors(out, exe_name):
    test_class_name = path.basename(exe_name) # The QTest class has the name as the executable

    ''' extract the interesting memcheck errors from the xml-string input 'out'.
    return these as a list '''
    xmldoc = parseString(out)
    errors = xmldoc.getElementsByTagName('error')
    errors_ = []
    for error in errors:
        bt = BackTrace(error)
        if bt.is_definitely_lost() and bt.is_test(test_class_name):
            errors_.append(bt)
    return errors_

def run_single_test(exe_name):
    print ">> running valgrind memcheck on " + exe_name
    system("export LD_LIBRARY_PATH="+sys.argv[2]+"/lib/:$LD_LIBRARY_PATH")
    count = 0
    import xml
    while count < 5:
        try:
            out = memcheck(exe_name)
            errors = parse_errors(out, exe_name)
            if len(errors) == 0:
                print "PASS"
                exit(0)
            else:
                for trace in errors:
                    print trace,
                    print "---------------------------------------------------"
                exit(-1)
        except xml.parsers.expat.ExpatError:
            print "Valgrind fooked up, retry"
            count += 1
            pass
    print "5 retries, no luck: aborting :("
    exit(-1)

################### ENTRY ####################################################


def isValgrind3_5OrHigher():
    process = Popen("valgrind --version", stdout=PIPE, shell=True)
    process.wait()
    valgrindOutput = process.stdout.read().strip()

    import re, string
    version = re.search("[0-9]+(.[0-9]+)*", valgrindOutput).group(0)
    version = string.split(version, ".")

    if map(int, version) < [3, 5]:
        return False
    else:
        return True

if __name__ == '__main__':
    if not isValgrind3_5OrHigher():
        print "Valgrind 3.5.0 or higher is needed. No mem check will be run."
        exit(-1)

    import sys
    run_single_test(sys.argv[1])
