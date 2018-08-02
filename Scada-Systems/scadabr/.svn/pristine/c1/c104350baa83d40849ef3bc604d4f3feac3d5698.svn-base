module.exports = function (grunt) {

    grunt.initConfig({
        jshint: ['desktop/**/*.js'],
        concat: {
            js: {
                files: {
                    'build.js': ['desktop/**/*.js']
                }
            }
        },
        uglify: {
            bundle: {
                files: {
                    'build.min.js': 'build.js'
                }
            }
        }
    });


    grunt.loadNpmTasks('grunt-contrib-jshint');

//  grunt.loadNpmTasks('grunt-contrib-less');
    grunt.loadNpmTasks('grunt-contrib-concat');
    grunt.loadNpmTasks('grunt-contrib-uglify');


    grunt.registerTask('js', 'Concatenate and minify static JavaScript assests',
            ['concat:js', 'uglify:bundle']);

    grunt.registerTask('default', ['jshint']);
};
