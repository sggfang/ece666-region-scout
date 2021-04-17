FROM spirals/parsec-3.0:source

RUN apt-get update && apt-get install -y \
	g++ \
	gettext \
	libtbb-dev \
	m4 \
	make \
	xorg-dev

RUN /home/parsec-3.0/bin/parsecmgmt -a fullclean -p all \
	&& /home/parsec-3.0/bin/parsecmgmt -a build

WORKDIR /home/parsec-3.0

ENTRYPOINT ["/bin/bash"]
