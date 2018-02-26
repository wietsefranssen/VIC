FROM ubuntu:xenial

# Dedicated work directory for output
ENV WORKDIR=$HOME/workdir
ENV PATH=$PATH:/VIC/vic
ENV USER="wietsefranssen_docker"

# Get the basic VIC dependencies
RUN apt-get update -q && \
    apt-get install -y -q --no-install-recommends \
        ca-certificates \
        build-essential \
        ssh \
        csh \
        netcdf-bin \
        libnetcdf-dev \
        openmpi-bin \
        libmpich-dev \
        git \
        cdo \
        nco \
        gdal-bin \
        make \
        zsh \
        dos2unix \
        curl \
        wget && \
        apt-get clean

# Install oh-my-zsh
RUN /bin/bash -c "$(wget https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh -O -)" && \
    mkdir -p $WORKDIR

# Put the UW-Hydro version of VIC in the container
    ARG SHA=LATEST
    RUN SHA=$(curl -s 'https://github.com/wietsefranssen/VIC/commits' | grep sha | head -1) && \
    git clone https://github.com/wietsefranssen/VIC.git && \
    cd VIC && \
    git remote add upstream  https://github.com/UW-Hydro/VIC.git && \
    git fetch upstream && \
    git checkout vicwur/docker && \
    cd ./vic && \
    make clean && \
    make

# Command to run when this image is "run", just build the classic and image drivers
CMD echo "#####################################################" && \
    echo "##" && \
    echo "## Welcome to the VIC-WUR environment!" && \
    echo "##" && \
    echo "## 'vicwur.exe' is compiled and ready to use" && \
    echo "## '/VIC' is the source-code folder"&& \
    echo "## '$WORKDIR' can be used as shared folder" && \
    echo "##" && \
    echo "## tools like cdo, nco & git are also ready to use" && \
    echo "##" && \
    echo "#####################################################" && \
    cd $WORKDIR && \
    zsh
