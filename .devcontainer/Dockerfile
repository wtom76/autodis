FROM mcr.microsoft.com/devcontainers/cpp:1-ubuntu-24.04

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.22.2"

# Optionally install the cmake for vcpkg
COPY ./reinstall-cmake.sh /tmp/

RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
RUN apt update && export DEBIAN_FRONTEND=noninteractive && apt -y install --no-install-recommends \
	gdb git pkg-config \
	libxmu-dev libxi-dev libgl-dev \
	linux-libc-dev \
	autoconf automake autoconf-archive libtool \
	bison flex \
	libudev-dev libx11-dev libxi-dev libxrandr-dev libxcursor-dev libgl1-mesa-dev \
	libsfml-dev \
#	python3-pip \
	&& apt clean && rm -rf /var/lib/apt/lists/*

#RUN pip install --break-system-packages tensorflow