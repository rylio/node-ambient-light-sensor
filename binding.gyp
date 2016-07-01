{
    'targets': [
        {
            'target_name': 'sensor',
            'sources': ['src/sensor.cc'],
            'include_dirs': ['<!(node -e \'require("nan")\')'],
            'conditions': [['OS=="mac"', {
                'xcode_settings': {
                    'OTHER_LDFLAGS': [
                        '-framework IOKit'
                    ]
                }
            }]]
        }
    ]
}
