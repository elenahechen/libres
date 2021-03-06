#  Copyright (C) 2012  Statoil ASA, Norway.
#
#  The file 'test_enkf_runpath.py' is part of ERT - Ensemble based Reservoir Tool.
#
#  ERT is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  ERT is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.
#
#  See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
#  for more details.


from ecl.test import ExtendedTestCase, TestAreaContext
from ecl.util import BoolVector

from res.enkf import (EnsembleConfig, AnalysisConfig, ModelConfig, SiteConfig,
                      EclConfig, PlotSettings, EnkfObs, ErtTemplates, EnkfFs,
                      EnKFState, EnkfVarType, ObsVector, RunArg, ResConfig)
from res.enkf.config import EnkfConfigNode
from res.enkf.enkf_main import EnKFMain
from res.enkf.enums import (EnkfObservationImplementationType, LoadFailTypeEnum,
                            EnkfInitModeEnum, ErtImplType, RealizationStateEnum,
                            EnkfRunType, EnkfFieldFileFormatEnum,
                            EnkfTruncationType, ActiveMode)

from res.enkf.observations.summary_observation import SummaryObservation

import os

class EnKFRunpathTest(ExtendedTestCase):
    def setUp(self):
        pass

    def test_with_gen_kw(self):
        case_directory = self.createTestPath('local/snake_oil_no_data/')
        with TestAreaContext('test_enkf_runpath', store_area=True) as work_area:
            work_area.copy_directory(case_directory)
            res_config = ResConfig('snake_oil_no_data/snake_oil.ert')
            main = EnKFMain(res_config)
            iactive = BoolVector(initial_size=main.getEnsembleSize(), default_value=False)
            iactive[0] = True
            fs = main.getEnkfFsManager().getCurrentFileSystem()
            run_context = main.getRunContextENSEMPLE_EXPERIMENT(fs, iactive)
            main.createRunpath(run_context)
            self.assertFileExists('storage/snake_oil/runpath/realisation-0/iter-0/parameters.txt')
            self.assertEqual(len(os.listdir('storage/snake_oil/runpath')), 1)
            self.assertEqual(len(os.listdir('storage/snake_oil/runpath/realisation-0')), 1)
            
    def test_without_gen_kw(self):
        case_directory = self.createTestPath('local/snake_oil_no_data/')
        with TestAreaContext('test_enkf_runpath', store_area=True) as work_area:
            work_area.copy_directory(case_directory)
            res_config = ResConfig('snake_oil_no_data/snake_oil_no_gen_kw.ert')
            main = EnKFMain(res_config)
            iactive = BoolVector(initial_size=main.getEnsembleSize(), default_value=False)
            iactive[0] = True
            fs = main.getEnkfFsManager().getCurrentFileSystem()
            run_context = main.getRunContextENSEMPLE_EXPERIMENT(fs, iactive)
            main.createRunpath(run_context)
            self.assertDirectoryExists('storage/snake_oil_no_gen_kw/runpath/realisation-0/iter-0')
            self.assertFileDoesNotExist('storage/snake_oil_no_gen_kw/runpath/realisation-0/iter-0/parameters.txt')
            self.assertEqual(len(os.listdir('storage/snake_oil_no_gen_kw/runpath')), 1)
            self.assertEqual(len(os.listdir('storage/snake_oil_no_gen_kw/runpath/realisation-0')), 1)
        
# eof 
