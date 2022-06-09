import sys
import os

def get_base_url():
    try:
        return os.environ['BASE_URL']
    except KeyError:
        print(
            '[error]: `BASE_URL` environment variable required\n')
        sys.exit(1)
        
def get_endpoint():
    try:
        return os.environ['ENDPOINT']
    except KeyError:
        print(
            '[error]: `ENDPOINT` environment variable required\n')
        sys.exit(1)
        
def get_raspi_p1():
    try:
        return int(os.environ['RASPI_P1'])
    except KeyError:
        print(
            '[error]: `RASPI_P1` environment variable required\n')
        sys.exit(1)
        
def get_raspi_p2():
    try:
        return int(os.environ['RASPI_P2'])
    except KeyError:
        print(
            '[error]: `RASPI_P2` environment variable required\n')
        sys.exit(1)


        
