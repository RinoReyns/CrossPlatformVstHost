import sys
from Modules.Enums import Status


def CheckStatus(status):
    if status != Status.SUCCESS.value:
        sys.exit(status)
