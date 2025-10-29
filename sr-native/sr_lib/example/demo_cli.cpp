//===========================================================================
// Super Resolution Sample app & Intel Proprietary Patch Software Licenses
//
// Super Resolution Sample app Software License
// Use and Redistribution. You may use, modify, and redistribute the software (the
// "Software"), solely for use on Intel platforms, provided the following conditions are met
// Redistributions must reproduce the above copyright notice and the following terms
// of use in the Software and in the documentation and/or other materials provided
// with the distribution.
// Neither the name of Intel nor the names of its suppliers may be used to endorse
// or promote products derived from this Software without specific prior written
// permission.
// Limited patent license. Intel grants you a world-wide, royalty-free, non-exclusive license
// under patents it now or hereafter owns or controls to make, have made, use, import,
// offer to sell and sell ("Utilize") this Software, but solely to the extent that any such patent
// is necessary to Utilize the Software alone. The patent license shall not apply to any
// combinations which include this software. No hardware per se is licensed hereunder.
//
// Third party programs. The Software may contain Third Party Programs. "Third Party
// Programs" are third party software, open source software or other Intel software listed
// in the "third-party-programs.txt" or other similarly named text file that is included with
// the Software. Third Party Programs, even if included with the distribution of the Software,
// may be governed by separate license terms, including without limitation, third party
// license terms, open source software notices and terms, and/or other Intel software
// license terms. These separate license terms may govern your use of the Third Party
// Programs.
// DISCLAIMER. THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
// ARE DISCLAIMED. THIS SOFTWARE IS NOT INTENDED FOR USE IN SYSTEMS OR
// APPLICATIONS WHERE FAILURE OF THE SOFTWARE MAY CAUSE PERSONAL INJURY OR
// DEATH AND YOU AGREE THAT YOU ARE FULLY RESPONSIBLE FOR ANY CLAIMS, COSTS,
// DAMAGES, EXPENSES, AND ATTORNEY'S FEES ARISING OUT OF ANY SUCH USE, EVEN
// IF ANY CLAIM ALLEGES THAT INTEL WAS NEGLIGENT REGARDING THE DESIGN OR
// MANUFACTURE OF THE MATERIALS.
//
// LIMITATION OF LIABILITY. IN NO EVENT WILL INTEL BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE. YOU AGREE TO INDEMNIFY AND HOLD INTEL HARMLESS AGAINST ANY
// CLAIMS AND EXPENSES RESULTING FROM YOUR USE OR UNAUTHORIZED USE OF THE
// SOFTWARE.
// No support. Intel may make changes to the Software, at any time without notice, and is
// not obligated to support, update or provide training for the Software.
// Termination. Intel may terminate your right to use the Software in the event of your
// breach of this Agreement and you fail to cure the breach within a reasonable period of
// time.
// Feedback. Should you provide Intel with comments, modifications, corrections,
// enhancements or other input ("Feedback") related to the Software Intel will be free to
// use, disclose, reproduce, license or otherwise distribute or exploit the Feedback in its sole
// discretion without any obligations or restrictions of any kind, including without limitation,
// intellectual property rights or licensing obligations.
// Compliance with laws. You agree to comply with all relevant laws and regulations
// governing your use, transfer, import or export (or prohibition thereof) of the Software.
//
// Governing law. All disputes will be governed by the laws of the United States of America
// and the State of Delaware without reference to conflict of law principles and subject to
// the exclusive jurisdiction of the state or federal courts sitting in the State of Delaware,
// and each party agrees that it submits to the personal jurisdiction and venue of those
// courts and waives any objections. The United Nations Convention on Contracts for the
// International Sale of Goods (1980) is specifically excluded and will not apply to the
// Software.
// *Other names and brands may be claimed as the property of others.
//--------------------------------------------------------------------------

// sr_cli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cstring>
#include "sr_lib.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: sr_cli.exe <-g> <-s 0/1> <-m 0/1/2>\n"
                  << " -g to get super-resolution status\n"
                  << " -s 0/1 to disable/enable super-resolution\n"
                  << " -m 0/1/2 to set super-resolution mode when enable super-resolution\n"
                  << "  0 - for normal video playback with removing encoding artifacts and"
                  << " smoothing edges;\n"
                  << "  1 - for but not limited to video conference related usage with both"
                  << " removing camera noises and has the strongest sharpen level;\n"
                  << "  2 - for surveillance/security camera usage that has both camera noises"
                  << " and the worst blocking/banding noises due to the network bandwidth limitation\n"
                  << std::endl;
    }

    bool bGet = false;
    bool bSet = false;
    bool bEnable = false;
    int mode = 0;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i] == NULL)
        {
            return -1;
        }

        if (!strcmp(argv[i], "-g"))
        {
            bGet = true;
        }
        else if (!strcmp(argv[i], "-s"))
        {
            if (++i < argc)
            {
                bEnable = !strcmp(argv[i], "1");
                bSet = true;
            }
        }
        else if (!strcmp(argv[i], "-m"))
        {
            if (++i < argc)
            {
                sscanf_s(argv[i], "%d", &mode);
            }
        }
    }

    if (!bSet && !bGet)
    {
        std::cerr << "Neither \"-g\" nor \"-s\" are specified" << std::endl;
        return -1;
    }

    SuperResolutionInitialize();

    if (bSet)
    {
        SuperResolutionSet(bEnable, mode);
    }

    if (bGet)
    {
        SuperResolutionGetStatus();
    }

    SuperResolutionRelease();

    std::cout << "Done." << std::endl;
}
