pipeline {
    agent none

    options {
        timestamps()
        buildDiscarder(logRotator(numToKeepStr: '20'))
    }

    stages {
        stage('Build & Test') {
            matrix {
                axes {
                    axis {
                        name 'OS_IMAGE'
                        values 'ubuntu:26.04', 'debian:13'
                    }
                    axis {
                        name 'BUILD_PRESET'
                        values 'gcc-release', 'clang-release'
                    }
                }

                excludes {
                    exclude {
                        axis {
                            name 'OS_IMAGE'
                            values 'debian:13'
                        }
                        axis {
                            name 'BUILD_PRESET'
                            values 'clang-release'
                        }
                    }
                }

                agent {
                    dockerfile {
                        filename 'ci/Dockerfile'
                        dir '.'
                        label 'linux && docker'
                        additionalBuildArgs "--build-arg BASE_IMAGE=${OS_IMAGE}"
                    }
                }

                stages {
                    stage('Configure') {
                        steps {
                            sh 'cmake --preset $BUILD_PRESET'
                        }
                    }

                    stage('Build') {
                        steps {
                            sh 'cmake --build --preset $BUILD_PRESET'
                        }
                    }

                    stage('Test') {
                        environment {
                            // No display in the container.
                            SDL_VIDEODRIVER = 'dummy'
                            SDL_AUDIODRIVER = 'dummy'
                        }
                        steps {
                            sh 'ctest --preset $BUILD_PRESET'
                        }
                    }
                }

                post {
                    always {
                        junit "build/${BUILD_PRESET}/test-results.xml"
                        cleanWs()
                    }
                }
            }
        }
    }
}
