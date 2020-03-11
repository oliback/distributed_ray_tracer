#!/bin/bash

if [[ ! -e ~/.ssh/id_rsa.pub ]]
then
	echo "Generating ssh key-pair."
	ssh-keygen -q -t rsa -N "" -f ~/.ssh/id_rsa
fi

touch ~/.ssh/authorized_keys

if grep -q "`cat ~/.ssh/id_rsa.pub`" ~/.ssh/authorized_keys
then
	echo "Keys are already set up."
	echo "Remember that to finalize the setup, you need to manually log in to the remote computers that you wish to use during the executions, using ssh on your workstation. (Answer \"yes\", you want to continue.)"
else
	echo "Setting up ssh keys."
	cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys
	echo "You now need to manually log in to the remote computers that you wish to use during the executions, using ssh on your workstation. (Answer \"yes\" you want to continue.)"
fi
