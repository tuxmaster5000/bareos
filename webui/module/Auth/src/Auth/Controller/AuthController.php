<?php

/**
 *
 * bareos-webui - Bareos Web-Frontend
 *
 * @link      https://github.com/bareos/bareos for the canonical source repository
 * @copyright Copyright (C) 2013-2024 Bareos GmbH & Co. KG (http://www.bareos.org/)
 * @license   GNU Affero General Public License (http://www.gnu.org/licenses/)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

namespace Auth\Controller;

use Zend\Mvc\Controller\AbstractActionController;
use Zend\View\Model\ViewModel;
use Zend\Session\Container;
use Exception;
use Auth\Model\Auth;
use Auth\Form\LoginForm;

class AuthController extends AbstractActionController
{
    /**
     * Variables
     */
    protected $directorModel = null;
    protected $bsock = null;
    protected $updates = null;

    /**
     * Index Action
     *
     * @return object
     */
    public function indexAction()
    {
        return new ViewModel();
    }

    /**
     * Login Action
     *
     * @return object
     */
    public function loginAction()
    {
        $multi_dird_env = false;

        if ($this->SessionTimeoutPlugin()->isValid()) {
            return $this->redirect()->toRoute('dashboard', array('action' => 'index'));
        }

        $this->layout('layout/login');

        $config = $this->getServiceLocator()->get('Config');
        $dird = $this->params()->fromQuery('dird') ? $this->params()->fromQuery('dird') : null;

        if (count($config['directors']) > 1) {
            $multi_dird_env = true;
        }

        $form = new LoginForm($config['directors'], $dird);

        $request = $this->getRequest();

        // Init Session Container
        $session = new Container('bareos');
        $configuration = $this->getServiceLocator()->get('configuration');
        $session->offsetSet('theme', $configuration['configuration']['theme']['name']);

        if (!$request->isPost()) {
            return $this->createNewLoginForm($form, $multi_dird_env);
        }

        $auth = new Auth();
        $form->setInputFilter($auth->getInputFilter());
        $form->setData($request->getPost());

        if (!$form->isValid()) {
            $err_msg = "Please provide a director, username and password.";
            return $this->createNewLoginForm($form, $multi_dird_env, $err_msg);
        }

        $director = $form->getInputFilter()->getValue('director');
        $username = $form->getInputFilter()->getValue('consolename');
        $password = $form->getInputFilter()->getValue('password');
        $locale = $form->getInputFilter()->getValue('locale');
        $bareos_updates = $form->getInputFilter()->getValue('bareos_updates');

        $config = $this->getServiceLocator()->get('Config');
        $this->bsock = $this->getServiceLocator()->get('director');
        $this->bsock->set_config($config['directors'][$director]);
        $this->bsock->set_user_credentials($username, $password);

        if (!$this->bsock->connect_and_authenticate()) {
            $err_msg = "Sorry, cannot authenticate. Wrong username, password or SSL/TLS handshake failed.";
            return $this->createNewLoginForm($form, $multi_dird_env, $err_msg, $this->bsock);
        }

        $session->offsetSet('director', $director);
        $session->offsetSet('username', $username);
        $session->offsetSet('password', $password);
        $session->offsetSet('authenticated', true);
        $session->offsetSet('locale', $locale);
        $session->offsetSet('idletime', time());
        $session->offsetSet('product-updates', null);
        $session->offsetSet('dird-update-info', null);
        $session->offsetSet('dt_lengthmenu', $configuration['configuration']['tables']['pagination_values']);
        $session->offsetSet('dt_pagelength', $configuration['configuration']['tables']['pagination_default_value']);
        $session->offsetSet('dt_statesave', ($configuration['configuration']['tables']['save_previous_state']) ? 'true' : 'false');
        $session->offsetSet('dashboard_autorefresh_interval', $configuration['configuration']['dashboard']['autorefresh_interval']);
        $session->offsetSet('filetree_refresh_timeout', $configuration['configuration']['restore']['filetree_refresh_timeout']);
        $session->offsetSet('merge_jobs', $configuration['configuration']['restore']['merge_jobs']);
        $session->offsetSet('merge_filesets', $configuration['configuration']['restore']['merge_filesets']);
        $session->offsetSet('configuration_resource_graph', $configuration['configuration']['experimental']['configuration_resource_graph']);

        if (isset($configuration['configuration']['autochanger']['labelpooltype'])) {
            $session->offsetSet('ac_labelpooltype', $configuration['configuration']['autochanger']['labelpooltype']);
        }

        if (
            $bareos_updates != "false"
        ) {
            $this->updates = json_decode($bareos_updates, true);
            $session->offsetSet('product-updates', $this->updates);
            $session->offsetSet('dird-update-info', $this->getUpdateInfoDIRD());
        }

        $apicheck = $this->checkAPIStatusDIRD();
        if ($apicheck !== true) {
            return $this->createNewLoginForm($form, $multi_dird_env, $apicheck, $this->bsock);
        }

        $versioncheck = $this->checkVersionCompatibilityDIRD();
        if ($versioncheck !== true) {
            return $this->createNewLoginForm($form, $multi_dird_env, $versioncheck, $this->bsock);
        }

        $aclcheck = $this->checkACLStatusDIRD();
        if (!$aclcheck) {
            return $this->createNewLoginForm($form, $multi_dird_env, $aclcheck, $this->bsock);
        } else {
            $session->offsetSet('commands', $aclcheck);
        }

        if ($this->params()->fromQuery('req')) {
            $redirect = $this->params()->fromQuery('req');
            $request = $this->getRequest();
            $request->setUri($redirect);
            if ($routeToBeMatched = $this->getServiceLocator()->get('Router')->match($request)) {
                return $this->redirect()->toUrl($this->params()->fromQuery('req'));
            }
        }

        return $this->redirect()->toRoute('dashboard', array('action' => 'index'));
    }

    /**
     * Logout Action
     *
     * @return object
     */
    public function logoutAction()
    {
        $session = new Container('bareos');
        $session->getManager()->destroy();
        return $this->redirect()->toRoute('auth', array('action' => 'login'));
    }

    /**
     * Get Director Model
     *
     * @return object
     */
    public function getDirectorModel()
    {
        if (!$this->directorModel) {
            $sm = $this->getServiceLocator();
            $this->directorModel = $sm->get('Director\Model\DirectorModel');
        }
        return $this->directorModel;
    }

    /**
     * Create New Login Form
     *
     * @return object
     */
    private function createNewLoginForm($form, $multi_dird_env = null, $err_msg = null, $bsock = null)
    {
        if ($bsock != null) {
            $bsock->disconnect();
        }
        session_destroy();
        return new ViewModel(
            array(
                'form' => $form,
                'multi_dird_env' => $multi_dird_env,
                'err_msg' => $err_msg
            )
        );
    }

    /**
     * DIRD version compatibility check
     *
     * @return mixed
     */
    private function checkVersionCompatibilityDIRD()
    {
        include 'version.php'; // provides bareos_full_version (installed ui version)
        $dird_version_array = $this->getDirectorModel()->getDirectorVersion($this->bsock);
        $dird_version = $dird_version_array['version'];
        // compare major version
        $dird_major_version = explode('.', $dird_version)[0];
        $ui_major_version = explode('.', $bareos_full_version)[0];
        if ($dird_major_version !== $ui_major_version) {
            $err_msg = 'Error: Bareos WebUI (' . $bareos_full_version . ') requires a Director of the same major release (' . $dird_major_version . '). The Director version is ' . $dird_version . '.';
            return $err_msg;
        }
        return true;
    }

    /**
     * DIRD API check
     *
     * @return mixed
     */
    private function checkAPIStatusDIRD()
    {
        $err_msg_1 = 'Sorry, the user you are trying to login with has no permissions for the .api command. For further information, please read the <a href="https://docs.bareos.org/IntroductionAndTutorial/InstallingBareosWebui.html#configuration-of-profile-resources" target="_blank">Bareos documentation</a>.';
        $err_msg_2 = 'Error: This Bareos Director does not support the required API 2 mode.';

        $result = $this->getDirectorModel()->sendDirectorCommand($this->bsock, ".api 2 compact=yes");

        // without permission for the api command, it returns:
        // .api: is an invalid command.
        if (preg_match('/.api:/', $result)) {
            return $err_msg_1;
        }

        // expected result: {"jsonrpc":"2.0","id":null,"result":{"api":2}}
        if (!preg_match('/"api": *2/', $result)) {
            return $err_msg_2;
        }

        return true;
    }

    /**
     * DIRD ACL check
     *
     * @return mixed
     */
    private function checkACLStatusDIRD()
    {
        $err_msg = 'Sorry, your Command ACL does not fit the minimal requirements. For further information, please read the <a href="https://docs.bareos.org/IntroductionAndTutorial/InstallingBareosWebui.html#configuration-of-profile-resources" target="_blank">Bareos documentation</a>.';

        try {
            $commands = $this->getDirectorModel()->getAvailableCommands($this->bsock);
        } catch (Exception $e) {
            echo $e->getMessage();
        }

        if ($commands['.help']['permission'] == 0) {
            return $err_msg;
        }

        return $commands;
    }


    /**
     * get update info for Bareos Director.
     *
     * @return mixed (false or array)
     */
    private function getUpdateInfoDIRD()
    {
        $dird_version = null;

        try {
            $dird_version_array = $this->getDirectorModel()->getDirectorVersion($this->bsock);
            if (array_key_exists('version', $dird_version_array)) {
                $dird_version = $dird_version_array['version'];
            }
        } catch (Exception $e) {
            echo $e->getMessage();
        }

        if (isset($dird_version)) {
            return \Bareos\Util::getNearestVersionInfo($this->updates, $dird_version);
        }

        return false;
    }
}
