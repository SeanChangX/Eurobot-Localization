"""Robot overrides may only override keys that exist in default.yaml."""

import glob
import os

from localization_bringup.params import param_files
import pytest
import yaml

PKG = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DEFAULT = os.path.join(PKG, 'config', 'default.yaml')
ROBOTS = sorted(glob.glob(os.path.join(PKG, 'config', 'robots', '*.yaml')))


def load(path):
    with open(path) as f:
        return yaml.safe_load(f) or {}


def test_default_has_parameters_for_every_node():
    for node, section in load(DEFAULT).items():
        assert 'ros__parameters' in section, node


@pytest.mark.parametrize('path', ROBOTS, ids=[os.path.basename(p) for p in ROBOTS])
def test_robot_overrides_only_known_keys(path):
    default = load(DEFAULT)
    for node, section in load(path).items():
        assert node in default, f'{path}: unknown node {node}'
        unknown = set(section['ros__parameters']) - set(default[node]['ros__parameters'])
        assert not unknown, f'{path}: {node} has parameters not in default.yaml: {unknown}'


def test_param_files_adds_override_only_when_present(tmp_path):
    (tmp_path / 'config' / 'robots').mkdir(parents=True)
    (tmp_path / 'config' / 'default.yaml').write_text('{}')
    (tmp_path / 'config' / 'robots' / 'white.yaml').write_text('{}')
    assert len(param_files(str(tmp_path), 'white')) == 2
    assert len(param_files(str(tmp_path), 'sim')) == 1
