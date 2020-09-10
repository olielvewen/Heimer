pipeline {
    agent none
    stages {
        stage('CMake Debug build and unit tests') {
            agent {
                docker {
                    image 'juzzlin/qt5-18.04:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "mkdir -p build-debug"
                sh "cd build-debug && cmake -DCMAKE_BUILD_TYPE=Debug .."
                sh "cd build-debug && cmake --build . --target all -- -j3 && ctest"
            }
        }
        stage('QMake build') {
            agent {
                docker {
                    image 'juzzlin/qt5-18.04:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "mkdir -p build-qmake"
                sh "cd build-qmake && qmake .. && make -j3"
            }
        }
        stage('Debian package / Ubuntu 16.04') {
            agent {
                docker {
                    image 'juzzlin/qt5-16.04:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "mkdir -p build-deb-ubuntu-16.04"
                sh "cd build-deb-ubuntu-16.04 && cmake -D DISTRO_VERSION=Ubuntu-16.04 -D CMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -D PACKAGE_TYPE=Deb .. && cmake --build . --target all -- -j3"
                sh "cd build-deb-ubuntu-16.04 && cpack -G DEB"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-deb-ubuntu-16.04/*.deb', fingerprint: true
                }
            }
        }
        stage('Debian package / Ubuntu 18.04') {
            agent {
                docker {
                    image 'juzzlin/qt5-18.04:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "mkdir -p build-deb-ubuntu-18.04"
                sh "cd build-deb-ubuntu-18.04 && cmake -D DISTRO_VERSION=Ubuntu-18.04  -D CMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -D PACKAGE_TYPE=Deb .. && cmake --build . --target all -- -j3"
                sh "cd build-deb-ubuntu-18.04 && cpack -G DEB"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-deb-ubuntu-18.04/*.deb', fingerprint: true
                }
            }
        }
        stage('Debian package / Ubuntu 20.04') {
            agent {
                docker {
                    image 'juzzlin/qt5-20.04:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "mkdir -p build-deb-ubuntu-20.04"
                sh "cd build-deb-ubuntu-20.04 && cmake -D DISTRO_VERSION=Ubuntu-20.04  -D CMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -D PACKAGE_TYPE=Deb .. && cmake --build . --target all -- -j3"
                sh "cd build-deb-ubuntu-20.04 && cpack -G DEB"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-deb-ubuntu-20.04/*.deb', fingerprint: true
                }
            }
        }
        stage('Build NSIS installer') {
            agent {
                docker {
                    image 'juzzlin/mxe-qt5-18.04:latest'
                    args '--privileged -t -v $WORKSPACE:/heimer'
                }
            }
            steps {
                sh "./scripts/build-windows-installer"
            }
            post {
                always {
                    archiveArtifacts artifacts: 'build-windows-docker/release/*.exe', fingerprint: true
                }
            }

        }
    }
}

