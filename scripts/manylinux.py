# -*- coding: utf-8 -*-
import os
from threading import Thread

import docker
from docker.client import DockerClient


def main(client: DockerClient) -> None:
    """
    Create wheels for all manylinux platforms.
    """

    def build_aarch64():
        print("""Building Manylinux aarch64 Wheels""")
        client.containers.run(
            "quay.io/pypa/manylinux2014_aarch64",
            "/io/scripts/manylinux.sh",
            platform="linux/arm64",
            volumes={os.getcwd(): {"bind": "/io"}},
        )

    def build_x86_64():
        print("""Building Manylinux x86_64 Wheels""")
        client.containers.run(
            "quay.io/pypa/manylinux2014_x86_64",
            "/io/scripts/manylinux.sh",
            volumes={os.getcwd(): {"bind": "/io"}},
        )

    def build_i686():
        print("""Building Manylinux i686 Wheels""")
        client.containers.run(
            "quay.io/pypa/manylinux2014_i686",
            "/io/scripts/manylinux.sh",
            volumes={os.getcwd(): {"bind": "/io"}},
        )

    aarch64 = Thread(target=build_aarch64)
    x86_64 = Thread(target=build_x86_64)
    i686 = Thread(target=build_i686)
    aarch64.start()
    x86_64.start()
    i686.start()
    aarch64.join()
    x86_64.join()
    i686.join()

    print("Wheels built Successfully.")


if __name__ == "__main__":
    main(docker.from_env())
