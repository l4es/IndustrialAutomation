@echo off
rem You should change the VCS to the one you use:
rem    --hg   - Mercurial
rem    --git  - Git
rem    --bzr  - Bazaar
rem    --p4   - Perforce
rem    --svn  - Subversion
python C:\Python25\Scripts\qct --p4 %*
