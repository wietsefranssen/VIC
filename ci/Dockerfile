FROM ubuntu:xenial

# Dedicated work directory for output
ENV WORKDIR $HOME/workdir
ENV PATH=$PATH:/VIC/vic

# Get the basic VIC dependencies
RUN apt-get update -q && \
    apt-get install -y -q --no-install-recommends \
        ca-certificates \
        build-essential \
        ssh \
        netcdf-bin \
        libnetcdf-dev \
        libopenmpi-dev \
        openmpi-bin \
        git \
        cdo \
        nco \
        make \
        zsh \
        wget

# Install oh-my-zsh
RUN /bin/bash -c "$(wget https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh -O -)" && \
    mkdir -p $WORKDIR

# Put the UW-Hydro version of VIC in the container
RUN git clone https://github.com/wietsefranssen/VIC.git && \
    cd VIC && \
    git checkout vicwur/docker && \
    cd ./vic && \
    make clean && \
    make

# Command to run when this image is "run", just build the classic and image drivers
#CMD git checkout develop && \
#    git pull origin develop && \
#    cd VIC/vic/drivers/classic && \
#    make && \
#    ./vic_classic.exe -o && \
#    cd ../image && \
#    make && \
#    ./vic_image.exe -o
CMD echo "Welcome..." && \
    cd $WORKDIR && \
    zsh
