#! /usr/bin/csh -f
#
#  Installing GIPSY from https://www.astro.rug.nl/~gipsy/installation/installation64.html
#  This requires too much reading in my opinion.  If your preconditions are correct,
#  and for me that's on Ubuntu, this script will automate the install.
#  If NEMO is loaded , this will be in $NEMO/opt/gipsy, else in your current dir.
#
#  After installation, you have about 234MB under the GIPSY root directory
#  This script should take about 3 minutes to install on a typical 2020 laptop
#
#  2022:   ? gfortran -fallow-argument-mismatch

if ($?NEMO) then
    mkdir -p $NEMO/opt/gipsy
    cd       $NEMO/opt/gipsy
endif    


echo sudo apt install gcc gfortran tcsh libxt-dev libncurses-dev libncurses-dev xaw3dg xaw3dg-dev libxaw7-dev -y
echo pip install https://www.astro.rug.nl/software/kapteyn/kapteyn-3.0.tar.gz
echo See also:  https://www.astro.rug.nl/~gipsy/installation/installation64.html
echo Sleeping for 5 seconds, then proceeding in `pwd`
sleep 5

# Step 0: Download the source (also a bad idea to not put a gipsy top level name?)
url=ftp://ftp.astro.rug.nl/gipsy/gipsy64/src/gipsy64python3_src.tar.gz
setenv gip_root `\pwd`
curl $url | tar zxf -
# wget $url -O -  | tar zxf -

# Installation step 1: Clients file
cd $gip_root/sys 
./mkclient.csh  71
mv clients.new $gip_root/loc/clients
#   what's the bash equivalent?
source cshrc.csh

# Installation step 2: Compiler setup
cd $gip_loc
cp $gip_sys/setup.mgr setup 
cd $gip_sys
#    this fails, gfortran needs the flag to allow old-style argument violations -fallow-argument-mismatch
./install.csh

# Installation step 3: Building the sources
cd $gip_sys
./mkbookkeeper.csh
mv bookkeeper.new bookkeeper

echo Now starting compilation, see $gip_sys/p.log
p -update > & p.log

# if locked, to unlock:
# rm -f $gip_sys/.*lock $gip_sys/*.lock $gip_tmp/update.lock

# Installation step 4: Local system setup
echo Sorry, not setting up legacy hardware.

# Final step, be nice to confused puppies

echo "setenv gip_root $gip_root"        > ../gipsy_start.csh
echo 'source $gip_root/sys/gipenv.csh' >> ../gipsy_start.csh

echo "export gip_root=$gip_root"        > ../gipsy_start.sh
echo 'source $gip_root/sys/gipenv.sh'  >> ../gipsy_start.sh

echo Created gipsy_start.csh and gipsy_start.sh in $gip_root
