 
echo_source_header()
{
    cat <<EOF 
/*
 * This file is part of HiKoB Openlab.
 * 
 * HiKoB Openlab is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, version 3.
 * 
 * HiKoB Openlab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with HiKoB Openlab. If not, see
 * <http://www.gnu.org/licenses/>.
 * 
 * Copyright (C) 2012 HiKoB.
 */ 
EOF
}

echo_config_header()
{
   cat <<EOF
# 
# This file is part of HiKoB Openlab.
# 
# HiKoB Openlab is free software: you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation, version 3.
# 
# HiKoB Openlab is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with HiKoB Openlab. If not, see
# <http://www.gnu.org/licenses/>.
# 
# Copyright (C) 2012 HiKoB.
# 
EOF
}


do_add_source_header()
{
    echo "$1"
    echo_source_header > tmp
    cp $1 $1.tmp
    cat tmp $1.tmp > $1
    rm tmp $1.tmp
}

do_check_author()
{
    if grep -q Author: $1 ; then
	echo -n ''
    else
	echo $1 #does not have any author
    fi
}

do_check_date()
{
    if grep -q 'Created on:' $1 ; then
	echo -n ''
    else
	echo $1
    fi
}

do_dates()
{
    echo -n ''
    #cat $* | grep 'Created on:' | sort | uniq
    #cat $* | echo "`grep ' _at_ '`" | sort | uniq
}

do_mod_authors()
{
    echo -n ''
   sed --in-place 's/burindes/Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>/g' $1
   sed --in-place 's/braillon/Christophe Braillon <christophe.braillon.at.hikob.com>/g' $1
   sed --in-place 's/afraboul/Antoine Fraboulet <antoine.fraboulet.at.hikob.com>/g' $1
   sed --in-place 's/groche/Guillaume Roche <guillaume.roche.at.hikob.com>/g' $1
   sed --in-place 's/ascherrer/Antoine Scherrer <antoine.scherrer.at.hikob.com>/g' $1
}

do_mod_fromdos()
{
    fromdos $1
}

do_mod_astyle()
{
    astyle $1
}

#LISTE=`find openlab -name 'CMakeLists.txt' -not -wholename '*os/freertos/*'`
LISTE=`find . -name '*.[hc]' -not -wholename '*os/freertos/*'`
#echo $LISTE

do_dates $LISTE
for i in $LISTE ; do 
    echo -n ''
    #echo "============== $i"
    #do_add_source $i
    #do_check_author   $i
    #do_check_date     $i
    #do_mod_authors    $i
    #do_mod_fromdos    $i
    #do_mod_authors    $i
    do_mod_astyle     $i
done


