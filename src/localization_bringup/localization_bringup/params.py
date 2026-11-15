"""Which parameter files a robot gets: config/default.yaml, then its own overrides.

A robot is configuration, not a git branch: everything robot-specific goes into
config/robots/<robot>.yaml and only overrides keys that exist in default.yaml
(test/test_config.py enforces that, so a typo cannot silently fall back to a default).
"""

import os


def param_files(share_dir, robot):
    files = [os.path.join(share_dir, 'config', 'default.yaml')]
    override = os.path.join(share_dir, 'config', 'robots', f'{robot}.yaml')
    if os.path.isfile(override):
        files.append(override)
    return files
