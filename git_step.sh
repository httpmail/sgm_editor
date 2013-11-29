#!/bin/bash
#
# Setup clean smudge filters
#
# Author aske.olsson@nokia.com
# Refactored for NG by Unto Holopainen on 2012-12-05

# Recursive function to set actual configuration
function setup_repo {
    local gitroot=$1

    cd $gitroot

    if  [  -d .git -o -f .git ]; then
        echo Setting configuration for $gitroot

        # Copy templates
        mkdir -p $(git rev-parse --git-dir)/templates
        cp -rf $toproot/tools/git/templates/* $(git rev-parse --git-dir)/templates/

        # Setup config for commit template
        if [ -z $notemplate ]; then
            git config commit.template "$(git rev-parse --git-dir)/templates/commit_template"
        else
            git config --unset commit.template
        fi

        # Setup config for colors
        if [ ! -z $colors ]; then

           if [ $OSTYPE == "linux" ]; then
               git config color.branch "auto"
               git config color.diff "auto"
               git config color.interactive "auto"
               git config color.status "auto"
           else
               echo "Colors already set in MINGW"
           fi
        fi

        # List repositories where we need to copy stuff
        if [ -f .gitmodules ]; then
            repolist=$(cat .gitmodules | grep path | sed -e 's/\s//g' | sed -e 's/path=//')
            # echo $repolist

            for repo in $repolist
            do
                if  [ -d $gitroot/$repo ]; then
                    # Setup submodule
                    setup_repo $gitroot/$repo
                else
                    echo Directory $gitroot does not exist, ignoring it
                fi
            done

            # exit 0
        fi
    else
        echo No git dir found from $gitroot, ignoring it
    fi
}

# Command line options
while [ "$1" != "" ]; do
    case $1 in
        -h*)
            usage
            exit 0
            ;;
        -no*)
            notemplate=1
            ;;
        -s*)
            shift
            site=$1
            ;;
        -ne*)
            next=next
            ;;
        -c*)
            colors=1
            ;;
        -*)
            usage
            exit 1
            ;;
        *)
            site=$1
            if [ ! -z $2 ]; then
                temp=$(echo "$2" | tr 'A-Z' 'a-z')
                if [ "$temp" = "next" ]; then
                    shift
                    next=next
                fi
            fi
            ;;
    esac
    shift
done

# Root dir
cd ./$(git rev-parse --show-cdup)

# Store root level directory for later use
toproot=$(pwd)
setup_repo $toproot
