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

        stage('Coverage') {
            agent {
                dockerfile {
                    filename 'ci/Dockerfile'
                    dir '.'
                    label 'linux && docker'
                    additionalBuildArgs '--build-arg BASE_IMAGE=ubuntu:26.04'
                }
            }

            steps {
                sh 'cmake --preset gcc-coverage'
                sh 'cmake --build --preset gcc-coverage'
                sh 'ctest --preset gcc-coverage'
                sh '''
                    mkdir -p build/gcc-coverage/coverage-html
                    gcovr --root . build/gcc-coverage \
                        --filter 'src/' --filter 'include/rspalgo/' --filter 'python/rspalgo_py.cpp' \
                        --exclude '.*/_deps/.*' --exclude '.*/test/.*' \
                        --cobertura build/gcc-coverage/coverage.xml \
                        --html --html-details -o build/gcc-coverage/coverage-html/index.html \
                        --print-summary
                '''
            }

            post {
                always {
                    junit 'build/gcc-coverage/test-results.xml'
                    recordCoverage tools: [[parser: 'COBERTURA', pattern: 'build/gcc-coverage/coverage.xml']]
                    publishHTML(target: [
                        reportDir: 'build/gcc-coverage/coverage-html',
                        reportFiles: 'index.html',
                        reportName: 'Coverage Report',
                        keepAll: true,
                        alwaysLinkToLastBuild: true
                    ])
                    cleanWs()
                }
            }
        }
    }
}
