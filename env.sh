[ -z $BOCHS_HOME ] && { echo "BOCHS_HOME not set"; exit 1; }

export PATH=$BOCHS_HOME/bin:$PATH
