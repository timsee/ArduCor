

function compile_doxygen()
{
    # remove previous versions
    rm -rf ../${1}
    mkdir ../${1}

    # run doxygen
    doxygen Doxyfile_${1}

    # if it contains a LaTeX version, compile as .pdf
    if [ -d ../${1}/latex ];
    then
        cd ../${1}/latex

        # build the LaTeX output
        make

        # copy the pdf
        cp -R refman.pdf ../${1}-API.pdf

        # remove LaTeX files
        cd ..
        rm -rf latex
    fi

    # go back to scripts folder
    cd ../doxygen
}


compile_doxygen "RoutinesRGB"
compile_doxygen "LightingGUI"
