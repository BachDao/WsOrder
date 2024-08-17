FROM ubuntu:noble

RUN \
	apt-get update && \
	apt-get install --no-install-recommends -y

# install updates & required packages
RUN \
	apt-get upgrade -y && \
	apt-get install --no-install-recommends -y \
		cmake \
        automake \
        build-essential \
        git \
        gcc \
        g++ \
        gdb \
        pkg-config \
        libboost-all-dev \
        libprotoc-dev \
        protobuf-compiler \
        libprotobuf-dev \
        libssl-dev \
        software-properties-common \
	&& apt-get clean && rm -rf /var/lib/apt/lists/*



