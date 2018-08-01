# Post-install script for Qct Windows MSI
# Configure default values for Qct preferences

from _winreg import HKEY_LOCAL_MACHINE, REG_SZ, CreateKey, SetValueEx, QueryValueEx

key = CreateKey(HKEY_LOCAL_MACHINE, 'Software\\vcs\\Qct\\tools')

try:
    value, type = QueryValueEx(key, 'diffTool')
except WindowsError:
    SetValueEx(key, 'diffTool', 0, REG_SZ, 'hg vdiff')

try:
    value, type = QueryValueEx(key, 'histTool')
except WindowsError:
    SetValueEx(key, 'histTool', 0, REG_SZ, 'hg view')

try:
    value, type = QueryValueEx(key, 'editTool')
except WindowsError:
    SetValueEx(key, 'editTool', 0, REG_SZ, 'notepad')
