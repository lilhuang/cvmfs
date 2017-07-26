#
# This file is part of the CernVM File System
# This script takes care of creating, removing, and maintaining repositories
# on a Stratum 0/1 server
#
# Implementation of the "cvmfs_server docker command"




cvmfs_server_docker() {

	local image_name="gitlab-registry.cern.ch/cvmfs/it-cvmfs-docker/cvmfs_container"
	local config_file_dir
	local repo_name
	local command_file


	# optional parameter handling
	# i: image name
	# c: config file directory (absolute path)
	# f: path to file for container to run as a command; can be as simple as just "/bin/bash"
	OPTIND=1
	while getopts "i:c:f:" option
	do
		case $option in
			i) 
				image_name=$OPTARG
			;;
			c) 
				config_file_dir=$OPTARG
			;;
			f)
				command_file=$OPTARG
			;;
			?) 
				shift $(($OPTIND-2))
				usage "Command docker: Unrecognized option: $1"
				exit
			;;
		esac
	done

	shift $((OPTIND-1))
	check_parameter_count_with_guessing $#
	# get repository names
	#currently only supports one repo at a time

	repo_name=$@

	docker pull $image_name

	if [ "$config_file_dir" == "" ] ; then
		if [ "$command_file" == "" ] ; then
			docker run -i -t -e REPO_NAME=$repo_name -v /var/spool/cvmfs --cap-add SYS_ADMIN --device /dev/fuse --rm $image_name
		else
			docker run -i -t -e REPO_NAME=$repo_name -v $command_file:/dockerfile_commands.sh -v /var/spool/cvmfs --cap-add SYS_ADMIN --device /dev/fuse --rm $image_name
		fi
	else
		if [ "$command_file" == "" ] ; then
			docker run -i -t -e REPO_NAME=$repo_name -v /var/spool/cvmfs -v $config_file_dir:/tmp/config_files:ro --cap-add SYS_ADMIN --device /dev/fuse --rm $image_name
		else
			docker run -i -t -e REPO_NAME=$repo_name -v $command_file:/dockerfile_commands.sh -v /var/spool/cvmfs -v $config_file_dir:/tmp/config_files:ro --cap-add SYS_ADMIN --device /dev/fuse --rm $image_name
		fi
	fi
}


